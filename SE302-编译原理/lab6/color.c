#include <stdio.h>
#include <string.h>

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
#include "table.h"

#define K  14 // number of allocable registers, because rsp an rbp can't fit to variables

// Data structures used in this file is referred to P176-P177 in textbook of Chinese version
// --------------------- Data structures needed for coloring ----------------
static G_nodeList precolored;       // set of machine registers
static G_nodeList initial;          // temp registers set, with elements not precolored or processed
static G_nodeList simplifyWorklist; // set of low-degree and move-irrelevant nodes
static G_nodeList freezeWorklist;   // set of low-degree and move-relevant nodes
static G_nodeList spillWorklist;    // set of high-degree nodes
static G_nodeList spilledNodes;     // set of nodes to be spilled in this round, initially empty
static G_nodeList coalescedNodes;   // set of coalesced nodes, when coalescing u<-v, add v into it
static G_nodeList coloredNodes;     // set of already colored nodes
static G_nodeList selectStack;      // stack having deleted nodes

// ----------------------- Set of move instructions needed ---------------------
static Live_moveList coalescedMoves;    // move instructions that are already coalesced
static Live_moveList constrainedMoves;  // move instructions with conflict src and dest nodes;
static Live_moveList frozenMoves;       // frozen move instructions
static Live_moveList worklistMoves;     // set of potential coalesced-enabled instructions
static Live_moveList activeMoves;       // instructioons read for coalescing

colorList ColorList(int c, colorList tail)
{
	colorList cl = checked_malloc(sizeof(*cl));
	cl->head = c;
	cl->tail = tail;
	return cl;
}

colorList initColorList()
{
	colorList cl = NULL;
	for (int k = K; k > 0; k--){
		cl = ColorList(k, cl);
	}
	return cl;
}

colorList delete_colorlist(int c, colorList cl)
{
	colorList result = cl;

	colorList *p;
	for (p = &result; *p; p = &((*p)->tail)){
		if ((*p)->head == c){
			*p = (*p)->tail;
			break;
		}
	}
	return result;
}

// ------------------------- other Data structures  --------------------------
static G_table adjSet;      // set of interference edges
static G_table adjList;     // adjencant table for the graph
static G_table degree;       // degree of nodes
static G_table moveList;    // a mapping for a node to moves related to it
static G_table alias;       // when (u,v) is coalesced, and v in coaleascedNodes, alias(v)=u
static G_table color;       // color for nodes

static string reg_nums[17] = {"none", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11","%r12", "%r13", "%r14", "%r15", "%rdi", "%rsi", "%rbp", "%rsp"};

// --------------------------- Auxiliary Functions -----------------------------
bool inNodeList(G_node node, G_nodeList list){
    while(list){
        if(node == list->head) return TRUE;
        else list = list->tail;
    }
    return FALSE;
}

bool inMoveList(Live_moveList ml_L, Live_moveList ml_R){
    while(ml_R){
        if((ml_L->src == ml_R->src && ml_L->dst == ml_R->dst)
            || (ml_L->src == ml_R->dst && ml_L->dst == ml_R->src))
            return TRUE;
        ml_R = ml_R->tail;
    }    
    return FALSE;   
}

G_nodeList unionNodeList(G_nodeList list1, G_nodeList list2){
    while(list1 != NULL){
        G_node head_node = list1->head;
        if(!inNodeList(head_node, list2)) list2 = G_NodeList(head_node, list2);
        list1 = list1->tail;
    }
    return list2;
}

G_nodeList subNodeList(G_nodeList list1, G_nodeList list2){
    G_nodeList new_list = NULL;
    while(list1){
        if(!inNodeList(list1->head, list2)){
            // the node in list1 is not in list2
            new_list = G_NodeList(list1->head, new_list);
        }
        list1 = list1->tail;
    }
    return new_list;
}


Live_moveList intersectionMoveList(Live_moveList ml_L, Live_moveList ml_R){
    Live_moveList overlap_ml = NULL;
    while(ml_L){
        if(inMoveList(ml_L, ml_R)) overlap_ml = Live_MoveList(ml_L->src, ml_L->dst, overlap_ml);
        ml_L = ml_L->tail;
    }
    return overlap_ml;
}

Live_moveList unionMoveList(Live_moveList ml_L, Live_moveList ml_R){
    // return ml_L \union ml_R
    while(ml_R){
        if(!inMoveList(ml_R, ml_L)){
            ml_L = Live_MoveList(ml_R->src, ml_R->dst, ml_L);
        }
        ml_R = ml_R -> tail;
    }
    return ml_L;
}

Live_moveList subMoveList(Live_moveList ml_L, Live_moveList ml_R){
    // return ml_L / ml_R
    Live_moveList diff_ml = NULL;
    while(ml_L){
        if(!inMoveList(ml_L, ml_R)){
            diff_ml = Live_MoveList(ml_L->src, ml_R->dst, diff_ml);
        }
        ml_L = ml_L->tail;
    }
    return diff_ml;
}

G_nodeList Adjacent(G_node node){
    // find adjacent nodes which are not in selectStack or coalesceNodes simulatenously
    G_nodeList allAdajacents = G_look(adjList, node);
    return subNodeList(allAdajacents, unionNodeList(selectStack, coalescedNodes));
}

Live_moveList NodeMoves(G_node n)
{
	Live_moveList ml = G_look(moveList, n);
	return intersectionMoveList(ml, unionMoveList(activeMoves, worklistMoves));
}

bool MoveRelated(G_node node){
    // check whether node is a move-related node
	return NodeMoves(node) != NULL;
}

void EnableMoves(G_nodeList nodes){
    while(nodes != NULL){
        G_node head_node = nodes->head;
        Live_moveList ml = NodeMoves(head_node);
        while(ml){
            Live_moveList move_pair = Live_MoveList(ml->src, ml->dst, NULL);
            if(inMoveList(move_pair, activeMoves)){
                assert(activeMoves != NULL);
                activeMoves = subMoveList(activeMoves, move_pair);
                worklistMoves = unionMoveList(worklistMoves, move_pair);
            }
            ml = ml->tail;
        }
        nodes = nodes->tail;
    }
}

void DecrementDegree(G_node n){
    int *degree_num = G_look(degree, n);
    assert(degree_num != NULL); // should found it

    int ori_degree_num = *degree_num;
    *degree_num -= 1;

    if(ori_degree_num == K){
        // after decrement, the degree of node becomes K-1, which enables simplify or coalescing
        EnableMoves(unionNodeList(G_NodeList(n, NULL), Adjacent(n)));
        spillWorklist = subNodeList(spillWorklist, G_NodeList(n, NULL));

        if(!inNodeList(n, precolored)){
            if(MoveRelated(n)) freezeWorklist = unionNodeList(freezeWorklist, G_NodeList(n, NULL));
            else simplifyWorklist = unionNodeList(simplifyWorklist, G_NodeList(n, NULL));
        }
    }
}

static void Initial(){
    // do necessary initialization on some data sturctures
	precolored = NULL;
	initial = NULL;
	simplifyWorklist = NULL;
	freezeWorklist = NULL;
	spillWorklist = NULL;
	spilledNodes = NULL;
	coalescedNodes = NULL;
	coloredNodes = NULL;
	selectStack = NULL;

	coalescedMoves = NULL;
	constrainedMoves = NULL;
	frozenMoves = NULL;
	worklistMoves = NULL;
	activeMoves = NULL;

	adjSet = G_empty();
	adjList = G_empty();
	degree = G_empty();
	moveList = G_empty();
	alias = G_empty();
	color = G_empty();
}

static void check_reg(Temp_temp temp, int* reg_num){
    // check which the reg is and return the reg index
    if(temp == F_RAX()) *reg_num = 1;
    else if(temp == F_RBX()) *reg_num = 2;
    else if(temp == F_RCX()) *reg_num = 3;
    else if(temp == F_RDX()) *reg_num = 4;
    else if(temp == F_R8()) *reg_num = 5;
    else if(temp == F_R9()) *reg_num = 6;
    else if(temp == F_R10()) *reg_num = 7;
    else if(temp == F_R11()) *reg_num = 8;
    else if(temp == F_R12()) *reg_num = 9;
    else if(temp == F_R13()) *reg_num = 10;
    else if(temp == F_R14()) *reg_num = 11;
    else if(temp == F_R15()) *reg_num = 12;
    else if(temp == F_RDI()) *reg_num = 13;
    else if(temp == F_RSI()) *reg_num = 14;
    else if(temp == F_RBP()) *reg_num = 15;
    else if(temp == F_RSP()) *reg_num = 16;
    else{
        printf("Build: failed in check_reg, unrecognized precolored register\n");
        assert(0);
    }
}

// ------------------ Primariy functional part --------

static void Build_precolored(G_nodeList nodes, Temp_map precolored_map){
    // precolored, initials and color
    while(nodes){
        G_node node = nodes->head;
        Temp_temp temp = Live_gtemp(node);
        if(Temp_look(precolored_map, temp) != NULL){
            // the temp is a precolored register
            precolored = G_NodeList(node, precolored);
            int *reg_number = checked_malloc(sizeof(int));
            check_reg(temp, reg_number);
            G_enter(color, node, reg_number);
        }
        else{
            initial = G_NodeList(node, initial);
        }
        nodes = nodes->tail;
    }
}

static void Build_adj_degree(G_nodeList nodes){
    // build table for ajacents and degree
    while (nodes){
		G_node node = nodes->head;
		G_nodeList adj_nodes = G_adj(node);
		G_enter(adjList, node, adj_nodes);
		if (Live_gtemp(node) == F_RAX()){
			printf("adjacents for the RAX:");
		}

		int *degree_num = checked_malloc(sizeof(int));
		*degree_num = 0;
		while (adj_nodes){
			(*degree_num)++;
			adj_nodes = adj_nodes->tail;
		}
		G_enter(degree, node, degree_num);
		nodes = nodes->tail;
	}
}

static void Build_worklist_moves(G_nodeList nodes, Live_moveList moves){
    Live_moveList temp_moves = moves;

	while (temp_moves){
		if (temp_moves->src != temp_moves->dst &&
				!inMoveList(temp_moves, worklistMoves)){
			worklistMoves = Live_MoveList(temp_moves->src, temp_moves->dst, worklistMoves);
		}
		temp_moves = temp_moves->tail;
	}

	temp_moves = worklistMoves;
	while (temp_moves){
		G_node src = temp_moves->src;
		G_node dst = temp_moves->dst;

		Live_moveList src_old = G_look(moveList, src);
		Live_moveList dst_old = G_look(moveList, dst);

		G_enter(moveList, src, Live_MoveList(src, dst, src_old));
		G_enter(moveList, dst, Live_MoveList(src, dst, dst_old));

		temp_moves = temp_moves->tail;
	}
}

static void Build(G_graph ig, Temp_map precolored_map, Live_moveList moves){
    Initial();  // initial all needed data sturctures

    G_nodeList nodes = G_nodes(ig);

    // step 1: build information for precolored, initials and color
    Build_precolored(nodes, precolored_map);

	// step 2: build adjList and degree
	Build_adj_degree(nodes);

	// step 3: moveList, worklistMoves
    Build_worklist_moves(nodes, moves);
}

static void MakeWorklist(){
    while(initial){
        G_node head_node = initial->head;
        int *degree_num = G_look(degree, head_node);

        if(*degree_num >= K) spillWorklist = G_NodeList(head_node, spillWorklist);
        else if(MoveRelated(head_node)) freezeWorklist= G_NodeList(head_node, freezeWorklist);
        else simplifyWorklist = G_NodeList(head_node, simplifyWorklist);

        initial = initial->tail;
    }
}

static void Simplify(){
    G_node node = simplifyWorklist->head;
    simplifyWorklist = simplifyWorklist->tail;

    selectStack = G_NodeList(node, selectStack);
    G_nodeList adjacent_nodes = Adjacent(node);

    while(adjacent_nodes!= NULL){
        DecrementDegree(adjacent_nodes->head);
        adjacent_nodes = adjacent_nodes->tail;
    }
}

void AddEdge(G_node u, G_node v){
    G_nodeList u_adjacents = G_look(adjList, u);
	G_nodeList v_adjacents = G_look(adjList, v);
    if(!inNodeList(v, u_adjacents) && u != v){
        u_adjacents = G_NodeList(v, u_adjacents);
        v_adjacents = G_NodeList(u, v_adjacents);
        G_enter(adjList, u, u_adjacents);
        G_enter(adjList, v, v_adjacents);

        if(!inNodeList(u, precolored)){
            int *u_degree = G_look(degree, u);
            assert(u_degree != NULL);
            *u_degree += 1;            
        }

        if(!inNodeList(v, precolored)){
            int *v_degree = G_look(degree, v);
            assert(v_degree != NULL);
            *v_degree += 1;
        }
    }
}

void AddWorkList(G_node u){
    int* degree_num = G_look(degree, u);
    assert(degree_num != NULL);

    bool inProcolored = inNodeList(u, precolored);
    bool moveRelated = MoveRelated(u);
    bool degree_lt_K = (*degree_num) < K;

    if(!inProcolored && !moveRelated && degree_lt_K){
        freezeWorklist = subNodeList(freezeWorklist, G_NodeList(u, NULL));
        simplifyWorklist = unionNodeList(simplifyWorklist, G_NodeList(u, NULL));
    }
}

bool OK(G_node t, G_node r){
    int* degree_num = G_look(degree, t);
    bool isPrecolored = inNodeList(t, precolored);
    bool isAdjacent = inNodeList(r, G_look(adjList, t));

    return (*degree_num) < K || isPrecolored || isAdjacent;
}

bool AdjacentOK(G_node v, G_node u){
    // check whether all adjacents of v, noting t satifys that OK(t,u)==TRUE
    G_nodeList v_adjacents = Adjacent(v);
    while(v_adjacents){
        G_node t = v_adjacents->head;
        if(!OK(t, u)){
            return FALSE;
        }
        v_adjacents = v_adjacents->tail;
    }
    return TRUE;
}

bool Conservative(G_nodeList nodes){
    int k = 0;
    while(nodes){
        G_node n = nodes->head;
        int* degree_num = G_look(degree, n);
        if((*degree_num) >= K) k += 1;
        nodes = nodes->tail;
    }
    return k < K;
}

G_node GetAlias(G_node n){
	if (!inNodeList(n, coalescedNodes)) return n;
    else{
		G_node m = G_look(alias, n);
		assert(m != NULL);
		return GetAlias(m);
	}
}

void Combine(G_node u, G_node v){
    if(inNodeList(v, freezeWorklist)){
        freezeWorklist = subNodeList(freezeWorklist, G_NodeList(v, NULL));
    }
    else{
        spillWorklist = subNodeList(spillWorklist, G_NodeList(v, NULL));
    }

    coalescedNodes = unionNodeList(coalescedNodes, G_NodeList(v, NULL));
    G_enter(alias, v, u);
    Live_moveList u_ml = G_look(moveList, u);
    Live_moveList v_ml = G_look(moveList, v);

    G_enter(moveList, u, unionMoveList(u_ml, v_ml));
    EnableMoves(G_NodeList(v, NULL));

    G_nodeList v_adjacents = Adjacent(v);
    while(v_adjacents){
        G_node t = v_adjacents->head;
        AddEdge(t, u);
        DecrementDegree(t);
        v_adjacents = v_adjacents->tail;
    }

    int *u_degree = G_look(degree, u);
    if((*u_degree) >= K && inNodeList(u, freezeWorklist) && !inNodeList(u, precolored)){
        freezeWorklist = subNodeList(freezeWorklist, G_NodeList(u, NULL));
        spillWorklist = unionNodeList(spillWorklist, G_NodeList(u, NULL));
    }
}

static void Coalesce(){
    Live_moveList m = Live_MoveList(worklistMoves->src, worklistMoves->dst, NULL);
    G_node x = GetAlias(worklistMoves->src);
    G_node y = GetAlias(worklistMoves->dst);

    G_node u, v;
    if(inNodeList(y, precolored)){
        u = y; v = x;
    }
    else{
        u = x; v = y;
    }

    worklistMoves = worklistMoves->tail;
    if(u == v){
        coalescedMoves = unionMoveList(coalescedMoves, m);
        AddWorkList(u);
    }
    else if(inNodeList(v, precolored) || inNodeList(v, G_look(adjList, u))){
        constrainedMoves = unionMoveList(constrainedMoves, m);
        AddWorkList(u);
        AddWorkList(v);
    }
    else if((inNodeList(u, precolored) && AdjacentOK(v, u)) 
         || (!inNodeList(u, precolored) && Conservative(unionNodeList(Adjacent(u), Adjacent(v))))
    ){
        coalescedMoves = unionMoveList(coalescedMoves, m);
        Combine(u, v);
        AddWorkList(u);
    }
    else{
        activeMoves = unionMoveList(activeMoves, m);
    }
}

void FreezeMoves(G_node u){
    Live_moveList ml = NodeMoves(u);
    while(ml){
        Live_moveList m = Live_MoveList(ml->src, ml->dst, NULL);
        G_node x = m->src;
        G_node y = m->dst;

        G_node v;
        if(GetAlias(y) == GetAlias(u)) v = GetAlias(x);
        else v = GetAlias(y);

        activeMoves = subMoveList(activeMoves, m);
        frozenMoves = unionMoveList(frozenMoves, m);

        int *v_degree = G_look(degree, v);
        if(NodeMoves(v) == NULL && (*v_degree) < K){
            freezeWorklist = subNodeList(freezeWorklist, G_NodeList(v, NULL));
            simplifyWorklist = unionNodeList(simplifyWorklist, G_NodeList(v, NULL));
        }

        ml = ml->tail;
    }
}

static void Freeze(){
	G_node u = freezeWorklist->head;
	freezeWorklist = freezeWorklist->tail;
	simplifyWorklist = unionNodeList(simplifyWorklist, G_NodeList(u, NULL));
	FreezeMoves(u);
}

static void SelectSpill(){
    G_nodeList candidates = spillWorklist;
    int max_degree = -1;
    G_node m = NULL;

    while(candidates){
        int *degree_num = G_look(degree, candidates->head);
        if((*degree_num) > max_degree){
            m = candidates->head;
            max_degree = *degree_num;
        }
        candidates = candidates->tail;
    }

    // select the node with max degree for spilling
    spillWorklist = subNodeList(spillWorklist, G_NodeList(m, NULL));
    simplifyWorklist = unionNodeList(simplifyWorklist, G_NodeList(m, NULL));
    FreezeMoves(m);
}

static void AssignColor(){
    while(selectStack){
        G_node n = selectStack->head;
        selectStack = selectStack->tail;
        
        colorList okColors = initColorList();
        G_nodeList n_adjacents = G_look(adjList, n);
        while(n_adjacents){
            G_node w = n_adjacents->head;
            G_node w_alias = GetAlias(w);
            assert(w_alias != NULL);
            if(inNodeList(w_alias, unionNodeList(coloredNodes, precolored))){
                int *color_index = G_look(color, w_alias);
                okColors = delete_colorlist(*color_index, okColors);
            }
            n_adjacents = n_adjacents->tail;
        }

        if(okColors == NULL) spilledNodes = unionNodeList(spilledNodes, G_NodeList(n, NULL));
        else{
            // not all colored used, allocate one to n
            coloredNodes = unionNodeList(coloredNodes, G_NodeList(n, NULL));
            int *c = checked_malloc(sizeof(int));
            *c = okColors->head;
            G_enter(color, n, c);
        }
    }

    // allocate same colors to aliases
    G_nodeList nodes = coalescedNodes;
    while(nodes){
        G_node n = nodes->head;
        int *c = G_look(color, GetAlias(n));
        assert(c != NULL);
        G_enter(color, n, c);
        nodes = nodes->tail;
    }
}

struct COL_result COL_color(G_graph ig, Temp_map precolored_map, Temp_tempList regs, Live_moveList moves) {
    // step 1: build
    Build(ig, precolored_map, moves);

    // step 2: make worklist for freezeWorklist, spillWorklist and simplifyWorklist
    MakeWorklist();

    while(1){
        if(simplifyWorklist != NULL) Simplify();
        else if(worklistMoves != NULL) Coalesce();
        else if(freezeWorklist != NULL) Freeze();
        else if(spillWorklist != NULL) SelectSpill();
        else break;
    }

    // step 3: allocate color 
    AssignColor();

    struct COL_result ret;

    Temp_map coloring_map = Temp_empty();
    G_nodeList colored_nodes = unionNodeList(coloredNodes, coalescedNodes);
    while(colored_nodes){
        G_node n = colored_nodes->head;
        int *c = G_look(color, n);
        assert(c != NULL);
        Temp_temp t = Live_gtemp(n);
        Temp_enter(coloring_map, t, String(reg_nums[*c]));
        colored_nodes = colored_nodes->tail;
    }                                                                                               

    // step 4: recored actual spilled nodes 
    Temp_tempList spills = NULL;
    G_nodeList actualSpills = spilledNodes;
    while(actualSpills){
        G_node spill_node = actualSpills->head;
        Temp_temp t = Live_gtemp(spill_node);
        spills = Temp_TempList(t, spills);
        actualSpills = actualSpills->tail;
    }

    ret.coloring = coloring_map;
    ret.spills = spills;
    return ret;
}
