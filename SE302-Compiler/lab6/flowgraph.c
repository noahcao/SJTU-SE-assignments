#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"
#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "flowgraph.h"
#include "errormsg.h"
#include "table.h"

Temp_tempList FG_def(G_node n) {
	AS_instr instr = (AS_instr)G_nodeInfo(n);
    if(instr->kind == I_OPER) return instr->u.OPER.dst;
    if(instr->kind == I_MOVE) return instr->u.OPER.dst;
	return NULL;
}

Temp_tempList FG_use(G_node n) {
	AS_instr instr = (AS_instr)G_nodeInfo(n);
    if(instr->kind == I_OPER) return instr->u.OPER.src;
    if(instr->kind == I_MOVE) return instr->u.OPER.src;
	return NULL;
}

bool FG_isMove(G_node n){
	AS_instr inst = G_nodeInfo(n);
	return inst->kind == I_MOVE;
}

G_graph FG_AssemFlowGraph(AS_instrList il, F_frame f){
	G_graph graph = G_Graph();
	TAB_table label_to_node = TAB_empty();

	Temp_labelList labels = NULL;
	G_node prev_node = NULL;

	for (AS_instrList instrs = il; instrs; instrs = instrs->tail){
		AS_instr inst = instrs->head;
		if (inst->kind == I_LABEL){
			labels = Temp_LabelList(inst->u.LABEL.label, labels);
			continue;
		}
		G_node cur_node = G_Node(graph, inst);

		// bind label to the following node
		while (labels){
			TAB_enter(label_to_node, labels->head, cur_node);
			labels = labels->tail;
		}

		// add edge
		if (prev_node != NULL) G_addEdge(prev_node, cur_node);

		// should not add egde when prev node is a jmp instr
		if (inst->kind == I_OPER && strncmp(inst->u.OPER.assem, "jmp", 3) == 0) prev_node = NULL;
		else prev_node = cur_node;
	}

	// add edge for jmping command
	G_nodeList nodes = G_nodes(graph);
	while (nodes){
		G_node node = nodes->head;
		AS_instr inst = G_nodeInfo(node);
		Temp_labelList target_labels = NULL;

		if (inst->kind == I_OPER && inst->u.OPER.jumps != NULL){
			target_labels = inst->u.OPER.jumps->labels;
		}

		while (target_labels){
			G_node jmp_node = TAB_look(label_to_node, target_labels->head);
			if (jmp_node != NULL) G_addEdge(node, jmp_node);

			target_labels = target_labels->tail;
		}
		nodes = nodes->tail;
	}
	return graph;
}
