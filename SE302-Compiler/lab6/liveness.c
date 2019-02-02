#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "flowgraph.h"
#include "liveness.h"
#include "table.h"

// --------------- Part 1: Auxiliary functions -------------
#define isFPorSP(x) (x==F_SP()||x==F_FP())  // whether it is fp/sp 

Temp_temp Live_gtemp(G_node n){
	return G_nodeInfo(n);
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail)
{
	Live_moveList lm = (Live_moveList)checked_malloc(sizeof(*lm));
	lm->src = src;
	lm->dst = dst;
	lm->tail = tail;
	return lm;
}

static bool inTempList(Temp_temp t, Temp_tempList l){
	if (l == NULL) return FALSE;
	if (l->head == t) return TRUE;
	return inTempList(t, l->tail);
}

static bool inNodeList(G_node n, G_nodeList l){
	if (l == NULL) return FALSE;
	if (l->head == n) return TRUE;
	return inNodeList(n, l->tail);
}

static Temp_tempList unionTempList(Temp_tempList L, Temp_tempList R){
	while (R){
		if (!inTempList(R->head, L)) L = Temp_TempList(R->head, L);
		R = R->tail;
	}
	return L;
}

static Temp_tempList subTempList(Temp_tempList L, Temp_tempList R){
	Temp_tempList t = NULL;
	while (L){
		if (!inTempList(L->head, R)) t = Temp_TempList(L->head, t);
		L = L->tail;
	}
	return t;
}

static bool sameTempList(Temp_tempList L, Temp_tempList R){
	Temp_tempList t;
	t = L;
	while (t){
		if (!inTempList(t->head, R)) return FALSE;
		t = t->tail;
	}

	t = R;
	while (t){
		if (!inTempList(t->head, L)) return FALSE;
		t = t->tail;
	}
	return TRUE;
}

// -------------------- Part 2: primary stages -----------

static void buildLiveness(G_graph flow, G_table livein_Map, G_table liveout_Map){
    // use iteration to update live-in and live-out information
    bool finish = FALSE;
    G_nodeList nodes;
	while (!finish){
		//begin refresh livein_Map and liveout_Map
		nodes = G_nodes(flow);
		finish = TRUE;
		
		while (nodes){
			G_node cur_node = nodes->head;
			// step 1: copy live-in and live-out from last iteration
            // in'[n] <- in[n], out'[n] <- out[n]
			Temp_tempList old_livein = *(Temp_tempList *)G_look(livein_Map, cur_node);
			Temp_tempList old_liveout = *(Temp_tempList *)G_look(liveout_Map, cur_node);

			Temp_tempList new_livein = old_livein;
			Temp_tempList new_liveout = old_liveout;

            // step 2: update live-in record
			// in[n] <- use[n] ∪ (out[n] - def[n])
			new_livein = unionTempList(FG_use(cur_node), subTempList(new_liveout, FG_def(cur_node)));

            // step 3: update live-out record
			// out[n] <- union all succ in[s], s is the succ of n
			G_nodeList succs = G_succ(cur_node);
			while (succs){
				Temp_tempList succ_in_temp = *(Temp_tempList *)G_look(livein_Map, succs->head);
				new_liveout = unionTempList(new_liveout, succ_in_temp);
				succs = succs->tail;
			}
  
            bool no_livein_update = sameTempList(old_livein, new_livein);
            bool no_liveout_update = sameTempList(old_liveout, new_liveout);
			if (!no_livein_update || !no_liveout_update) finish = FALSE;

			*(Temp_tempList *)G_look(livein_Map, cur_node) = new_livein;
			*(Temp_tempList *)G_look(liveout_Map, cur_node) = new_liveout;

			nodes = nodes->tail;
		}
	}
}

static void buildIntefGraph(G_graph flow, G_graph interference_graph, TAB_table temp_to_node){
	G_nodeList nodes = G_nodes(flow);
	while (nodes != NULL){
		Temp_tempList use_tempList = FG_use(nodes->head);
		Temp_tempList def_tempList = FG_def(nodes->head);

		while (def_tempList){
			Temp_temp temp = def_tempList ->head;
            def_tempList = def_tempList ->tail;

			if (isFPorSP(temp)) continue;

			if (TAB_look(temp_to_node, temp) == NULL){
                // add new def information
				G_node temp_node = G_Node(interference_graph, temp);
				TAB_enter(temp_to_node, temp, temp_node);
			}
		}
        
		while (use_tempList){
			Temp_temp temp = use_tempList ->head;
            use_tempList = use_tempList ->tail;

			if (isFPorSP(temp)) continue;

			if (TAB_look(temp_to_node, temp) == NULL){
                // add new use information
				G_node temp_node = G_Node(interference_graph, temp);
				TAB_enter(temp_to_node, temp, temp_node);
			}
		}
		nodes = nodes->tail;
	}
}

static void addEdge(G_graph flow, G_table livein_Map, G_table liveout_Map, TAB_table temp_to_node){
    G_nodeList nodes = G_nodes(flow);
	while (nodes){
		Temp_tempList defs = FG_def(nodes->head);
		Temp_tempList uses = FG_def(nodes->head);
		

		while (defs){
			Temp_temp def_temp = defs->head;
            Temp_tempList live_outs = *(Temp_tempList *)G_look(liveout_Map, nodes->head);
			G_node def_node = TAB_look(temp_to_node, def_temp);
            defs = defs->tail;

			if (isFPorSP(def_temp)) continue;

			if (FG_isMove(nodes->head)){
				while (live_outs){
					Temp_temp out_temp = live_outs->head;
					G_node out_node = TAB_look(temp_to_node, out_temp);
                    live_outs = live_outs->tail;

					if (isFPorSP(out_temp)) continue;

					// avoid add same edge more than once
					if ((out_node != def_node) &&
							!inNodeList(out_node, G_adj(def_node)) &&
							!inTempList(out_temp, uses)){
						G_addEdge(def_node, out_node);
					}
				}
			}
			else{
				while (live_outs){
					Temp_temp out_temp = live_outs->head;
					G_node out_node = TAB_look(temp_to_node, out_temp);
                    live_outs = live_outs->tail;
                    
					if (isFPorSP(out_temp)) continue;

					// avoid add same edge more than once
					if ((out_node != def_node) &&
							!inNodeList(out_node, G_adj(def_node))){
						G_addEdge(def_node, out_node);
					}
				}
			}
		}
		nodes = nodes->tail;
	}
}

Live_moveList buildMoveList(G_graph flow, TAB_table temp_to_node){
    Live_moveList moveList = NULL;
	G_nodeList nodes = G_nodes(flow);
	while (nodes){
		if (FG_isMove(nodes->head)){
			AS_instr instr = G_nodeInfo(nodes->head);

			if (!isFPorSP(instr->u.MOVE.src->head) &&
					!isFPorSP(instr->u.MOVE.dst->head)){
				G_node src = TAB_look(temp_to_node, instr->u.MOVE.src->head);
                G_node dst = TAB_look(temp_to_node, instr->u.MOVE.dst->head);
                assert(src != NULL);
                assert(dst != NULL);

				if (src != dst) moveList = Live_MoveList(src, dst, moveList);
			}
		}
		nodes = nodes->tail;
	}
    return moveList;
}

struct Live_graph Live_liveness(G_graph flow){
	struct Live_graph lg;
	G_table livein_Map = G_empty();
	G_table liveout_Map = G_empty();
    G_nodeList nodes = G_nodes(flow);

	while (nodes){
        // initialize the live-in and live-out maps
		G_enter(livein_Map, nodes->head, checked_malloc(sizeof(Temp_tempList)));
		G_enter(liveout_Map, nodes->head, checked_malloc(sizeof(Temp_tempList)));
		nodes = nodes->tail;
	}

    // step 1: build live-in and live-out map
    buildLiveness(flow, livein_Map, liveout_Map);

    // step 2: build interfering graph
	G_graph interference_g = G_Graph();
	TAB_table temp_to_node = TAB_empty();
    buildIntefGraph(flow, interference_g, temp_to_node);

    // step 3: add edges between interfering nodes
    addEdge(flow, livein_Map, liveout_Map, temp_to_node);

    // step 4: build the move list
	Live_moveList moves = buildMoveList(flow, temp_to_node);

	lg.graph = interference_g;
	lg.moves = moves;
	return lg;
}
