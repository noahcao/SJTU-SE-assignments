#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "liveness.h"
#include "color.h"
#include "regalloc.h"
#include "table.h"
#include "flowgraph.h"
#include <string.h>

#define F_wordSize 8

static bool inTempList(Temp_temp t, Temp_tempList l){
	if (l == NULL) return FALSE;
	if (l->head == t) return TRUE;

	return inTempList(t, l->tail);
}

static Temp_tempList replaceTemp(Temp_temp old, Temp_temp new, Temp_tempList tl){
	if (tl == NULL) return tl;
	if (tl->head == old) return Temp_TempList(new, replaceTemp(old, new, tl->tail));
	else return Temp_TempList(tl->head, replaceTemp(old, new, tl->tail));
}

AS_instrList cleanRBP(AS_instrList il, int frame_size){
    // function to remove use of %rbp
    AS_instrList rewrited_il = il;
    for(; il != NULL; il = il->tail){
        AS_instr inst = il->head;
        if (inst->kind == I_OPER){
            // case 1: movq %d(%rbp), `d0
            if (inst->u.OPER.src && inst->u.OPER.src->head == F_FP()){
                string assem = inst->u.OPER.assem;
                char inst_type[20];
                int num;
                sscanf(assem, "%s %d", inst_type, &num);

                if (strcmp(inst_type, "movq") == 0){
                    char buf[256];
                    sprintf(buf, "movq %d(%rsp), `d0", frame_size + num);
                    il->head = AS_Oper(String(buf), inst->u.MOVE.dst, NULL, NULL);
                }
            }
            // case 2: movq `s0, %d(%rbp)
            else if (inst->u.OPER.src && inst->u.OPER.src->tail && inst->u.OPER.src->tail->head == F_FP()){
                string assem = inst->u.OPER.assem;
                char inst_type[20];
                char source[20];
                int num;

                sscanf(assem, "%s %s %d", inst_type, source, &num);
                if (strcmp(inst_type, "movq") == 0){
                    char buf[256];
                    sprintf(buf, "movq `s0, %d(%rsp)", frame_size + num);
                    il->head = AS_Oper(String(buf), NULL, inst->u.MOVE.src, NULL);
                }
            }
        }
        else if (inst->kind == I_MOVE && inst->u.MOVE.src->head == F_FP()){
            // case 3: leaq %d(%rsp), `d0
            char buf[256];
            sprintf(buf, "leaq %d(%rsp), `d0", frame_size);
            il->head = AS_Oper(String(buf), inst->u.MOVE.dst, NULL, NULL);
        }
    }
    return rewrited_il;
}

static AS_instrList RewriteProgram(F_frame f, AS_instrList il, Temp_tempList spills){
    // rewrite programs for handling spills using load/store instructions
	AS_instrList rewrited_il = il;
	while (spills){
		Temp_temp spill_temp = spills->head;
		spills = spills->tail;
		f->length += F_wordSize;

		AS_instrList target_il = rewrited_il;
		for(; target_il != NULL; target_il = target_il->tail){
			AS_instr inst = target_il->head;
			Temp_tempList src_templist = NULL;
			Temp_tempList dst_templist = NULL;

			if (inst->kind == I_MOVE){
				src_templist = inst->u.MOVE.src;
				dst_templist = inst->u.MOVE.dst;
			}
			else if (inst->kind == I_OPER){
				src_templist = inst->u.OPER.src;
				dst_templist = inst->u.OPER.dst;
			}

			// for definition, add store after
			if (inTempList(spill_temp, dst_templist)){
				Temp_temp t = Temp_newtemp();
				Temp_tempList new_tl = replaceTemp(spill_temp, t, dst_templist);

				if (inst->kind == I_MOVE) inst->u.MOVE.dst = new_tl;
				else if (inst->kind == I_OPER) inst->u.OPER.dst = new_tl;

				// store
				char buf[256];
				sprintf(buf, "movq `s0, %d(`s1)", -f->length);
				AS_instr store_instr = AS_Oper(String(buf), NULL, Temp_TempList(t, Temp_TempList(rbp, NULL)), NULL);

				target_il->tail = AS_InstrList(store_instr, target_il->tail);
			}
			else if (inTempList(spill_temp, src_templist)){
				Temp_temp t = Temp_newtemp();
				Temp_tempList new_tl = replaceTemp(spill_temp, t, src_templist);

				if (inst->kind == I_MOVE) inst->u.OPER.src = new_tl;
				else if (inst->kind == I_OPER) inst->u.OPER.src = new_tl;

				// load
				char buf[256];
				sprintf(buf, "movq %d(`s0), `d0", -f->length);
				AS_instr load_instr = AS_Oper(String(buf), Temp_TempList(t, NULL), Temp_TempList(rbp, NULL), NULL);

				target_il->head = load_instr;
				target_il->tail = AS_InstrList(inst, target_il->tail);
				target_il = target_il->tail; // avoid dead loop
			}
		}
	}

	return rewrited_il;
}

struct RA_result RA_regAlloc(F_frame f, AS_instrList il){
	// step 1: generate flowgraph and do liveness analysis
    G_graph flowgraph = FG_AssemFlowGraph(il, f);
    G_nodeList instrs = G_nodes(flowgraph);

    struct Live_graph liveness_graph = Live_liveness(flowgraph);
    G_nodeList nodes = G_nodes(liveness_graph.graph);

	// step 2: do coloring
    struct COL_result color_result = COL_color(liveness_graph.graph, F_tempMap, NULL, liveness_graph.moves);

    if(color_result.spills != NULL){
        // actual spillings occurs during coloring
        AS_instrList rewrited_il = RewriteProgram(f, il, color_result.spills);
        return RA_regAlloc(f, rewrited_il);
    }

    // step 3: handling move instruction with same dst and src
	Temp_map outputMap = Temp_layerMap(F_tempMap, Temp_layerMap(color_result.coloring, Temp_name()));
	AS_instrList *p = &il;
	for(; *p != NULL; p = &((*p)->tail)){
		AS_instr instr = (*p)->head;
		if (instr->kind == I_MOVE){
			char *src = Temp_look(outputMap, instr->u.MOVE.src->head);
			char *dst = Temp_look(outputMap, instr->u.MOVE.dst->head);
            printf("MOVE: %s %s -> %s\n", instr->u.MOVE.assem, src, dst);

			if (strncmp(src, dst, 4) == 0){
				*p = (*p)->tail;
                printf("Delete one\n");
				continue;
			}
		}
        else if(instr->kind == I_LABEL){
            printf("LABEL: %s\n", instr->u.LABEL.assem);
        }
        else if(instr->kind == I_OPER){
            printf("OPER: %s\n", instr->u.OPER.assem);
        }
        else{
            assert(0);
        }
	}

    // step 4: finally, remove all rbp as required in this lab
    AS_instrList il_nofp = cleanRBP(il, f->length);

    struct RA_result ret;
	ret.coloring = color_result.coloring;
	ret.il = il_nofp;
	return ret;
}
