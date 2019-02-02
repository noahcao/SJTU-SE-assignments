#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "util.h"
#include "absyn.h"
#include "temp.h"
#include "frame.h"

/* Lab5: your code below */
//typedef struct patchList_ *patchList;
typedef struct Tr_exp_ *Tr_exp;
typedef struct Tr_expList_ *Tr_expList;
typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;
typedef struct Tr_level_ *Tr_level;

struct Tr_access_ {
	Tr_level level;
	F_access access;
};


struct Tr_accessList_ {
	Tr_access head;
	Tr_accessList tail;	
};

struct Tr_level_ {
	F_frame frame;
	Tr_level parent;
};

typedef struct patchList_ *patchList;
struct patchList_ 
{
	Temp_label *head; 
	patchList tail;
};

struct Cx 
{
	patchList trues; 
	patchList falses; 
	T_stm stm;
};

struct Tr_exp_ {
	enum {Tr_ex, Tr_nx, Tr_cx} kind;
	union {T_exp ex; T_stm nx; struct Cx cx; } u;
};

struct Tr_expList_ {Tr_exp head; Tr_expList tail;};

//struct patchList_ { Temp_label *head; patchList tail; };
//struct Cx { T_stm stm; patchList trues; patchList falses; };
//struct Tr_exp_ {
//    enum { Tr_ex, Tr_nx, Tr_cx } kind;
//    union {
//        T_exp ex;
//        T_stm nx;
//        struct Cx cx;
//    } u;
//};
//struct Tr_access_ { Tr_level level; F_access access; };
//struct Tr_accessList_ { Tr_access head; Tr_accessList tail; };
//struct Tr_expList_ {Tr_exp head; Tr_expList tail;};

// constructors for pratique
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);
Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail);

Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals);

// statements cases
Tr_exp Tr_assign(Tr_exp lvalue, Tr_exp value);
Tr_exp Tr_ifthen(Tr_exp test, Tr_exp then);
Tr_exp Tr_while(Tr_exp test, Tr_exp body, Temp_label done);
Tr_exp Tr_for(Tr_access loop_var_access, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done);
Tr_exp Tr_break(Temp_label done);
Tr_exp Tr_condition(A_oper oper, Tr_exp left, Tr_exp right);

// expressions cases
Tr_exp Tr_simpleVar(Tr_access access, Tr_level level);
Tr_exp Tr_fieldVar(Tr_exp ptr, int field_order);  
Tr_exp Tr_arrayVar(Tr_exp ptr, Tr_exp index);  
Tr_exp Tr_nil();
Tr_exp Tr_int(int value);
Tr_exp Tr_string(string str);
Tr_exp Tr_call(Temp_label func, Tr_expList args, Tr_level caller, Tr_level callee);
Tr_exp Tr_arithmetic(A_oper oper, Tr_exp left, Tr_exp right);
Tr_exp Tr_recordCreation(Tr_expList fields);
Tr_exp Tr_eseq(Tr_exp first, Tr_exp second);
Tr_exp Tr_ifthenelse(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_arrayCreation(Tr_exp size_exp, Tr_exp init_exp);

F_fragList Tr_getResult(void);


#endif
