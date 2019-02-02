/*
 * color.h - Data structures and function prototypes for coloring algorithm
 *             to determine register allocation.
 */

#ifndef COLOR_H
#define COLOR_H

#include "temp.h"
#include "graph.h"
#include "liveness.h"

struct COL_result {Temp_map coloring; Temp_tempList spills;};
struct COL_result COL_color(G_graph ig, Temp_map precolored_map, Temp_tempList regs, Live_moveList moves);

typedef struct colorList_ *colorList;

struct colorList_
{
	int head;
	colorList tail;
};

#endif
