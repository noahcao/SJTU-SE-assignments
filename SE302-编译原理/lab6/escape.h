#ifndef __ESCAPE_H

#define __ESCAPE_H

#include "absyn.h"
#include "symbol.h"
#include "helper.h"

typedef struct E_escapeEntry_ *E_escapeEntry;
struct E_escapeEntry_
{
  int depth;
  bool *escape;
};

E_escapeEntry E_EscapeEntry(int depth, bool *escape);

void Esc_findEscape(A_exp exp);

static void traverseExp(S_table env, int depth, A_exp e);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);
#endif