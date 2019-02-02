#ifndef LIVENESS_H
#define LIVENESS_H

typedef struct Live_moveList_ *Live_moveList;
struct Live_moveList_ {
	G_node src, dst;
	Live_moveList tail;
};

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail);

struct Live_graph {
	G_graph graph;
	Live_moveList moves;
};
Temp_temp Live_gtemp(G_node n);

struct Live_graph Live_liveness(G_graph flow);
/*
typedef struct Live_moveList_ *Live_moveList;
struct Live_moveList_ {
	G_node src, dst;
	Live_moveList tail;
};

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail);

typedef struct Live_graph_ *Live_graph;
struct Live_graph_ {
	G_graph graph;
	Live_moveList moves;
};
Live_graph Live_liveness(G_graph flow);
Temp_temp Live_gtemp(G_node n);
bool findTemp(Temp_tempList t, Temp_temp temp);
Temp_tempList addTemp(Temp_tempList t, Temp_temp temp);
void changeTemp(Temp_tempList t, Temp_temp old, Temp_temp n);
void showInterference(G_graph conflict_node_map, Live_moveList moves);*/
#endif
