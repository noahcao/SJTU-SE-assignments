#include <stdio.h>
#include "util.h"
#include "table.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "printtree.h"
#include "frame.h"
#include "translate.h"
#include "errormsg.h"

//LAB5: you can modify anything you want.

static F_fragList strfragList = NULL;
static F_fragList funcfragList = NULL;
static int F_wordsize = 8;

static Tr_exp ex(T_exp exp){
    Tr_exp tr_exp = checked_malloc(sizeof(*tr_exp));
    tr_exp->kind = Tr_ex;
    tr_exp->u.ex = exp;
    return tr_exp;
}

static Tr_exp nx(T_stm stm){
   Tr_exp tr_exp = checked_malloc(sizeof(*tr_exp));
    tr_exp->kind = Tr_nx;
    tr_exp->u.nx = stm;
    return tr_exp;
}

static Tr_exp cx(struct Cx cx) {
    Tr_exp tr_exp = checked_malloc(sizeof(*tr_exp));
    tr_exp->kind = Tr_cx;
    tr_exp->u.cx = cx;
    return tr_exp;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm)
{
	Tr_exp exp = checked_malloc(sizeof(*exp));

	exp->kind = Tr_cx;
	exp->u.cx.trues = trues;
	exp->u.cx.falses = falses;
	exp->u.cx.stm = stm;
	return exp;
}

static patchList PatchList(Temp_label *head, patchList tail){
	patchList list;
	list = (patchList)checked_malloc(sizeof(struct patchList_));
	list->head = head;
	list->tail = tail;
	return list;
}

void doPatch(patchList tList, Temp_label label)
{
	for (; tList; tList = tList->tail)
		*(tList->head) = label;
}

patchList joinPatch(patchList first, patchList second)
{
	if(!first) return second;
	for(; first->tail; first = first->tail);
	first->tail = second;
	return first;
}

static T_exp unEx(Tr_exp e){
	// transform exp of any type to a ex-exp
	switch (e->kind){
		case Tr_ex: return e->u.ex;
		case Tr_nx: return T_Eseq(e->u.nx, T_Const(0));
		case Tr_cx:{
			Temp_temp r = Temp_newtemp();
			Temp_label t = Temp_newlabel();
			Temp_label f = Temp_newlabel();
			doPatch(e->u.cx.trues, t);
            doPatch(e->u.cx.falses, f);
			return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
                          T_Eseq(e->u.cx.stm,
                                 T_Eseq(T_Label(f),
                                        T_Eseq(T_Move(T_Temp(r), T_Const(0)),
                                               T_Eseq(T_Label(t), T_Temp(r))))));
		}
		default: assert(0);
	}
}

static T_stm unNx(Tr_exp e){
	// transform exp of any type to a ux-exp
	switch (e->kind){
		case Tr_ex: return T_Exp(e->u.ex);
		case Tr_nx: return e->u.nx;
		case Tr_cx: {
			Temp_label label = Temp_newlabel();
			doPatch(e->u.cx.trues, label);
			doPatch(e->u.cx.falses, label);
			return T_Seq(e->u.cx.stm, T_Label(label));
		}
		default: assert(0);
	}
}

static struct Cx unCx(Tr_exp e){
	switch (e->kind){
        case Tr_ex:{
            struct Cx cx;
            T_stm s = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL);
            patchList trues = PatchList(&(s->u.CJUMP.true), NULL);
            patchList falses = PatchList(&(s->u.CJUMP.false), NULL);

            cx.stm = s;
            cx.trues = trues;
            cx.falses = falses;

            return cx;
        }
        case Tr_cx: return e->u.cx;
        case Tr_nx: assert(0);
	}
}

static Tr_accessList makeTrAccessList(F_accessList list, Tr_level level){
	if(list == NULL) return NULL;

	Tr_access t_access = checked_malloc(sizeof(*t_access));
	t_access->level = level;
	t_access->access = list->head;

	return Tr_AccessList(t_access, makeTrAccessList(list->tail, level));
}

static T_expList makeTExpList(Tr_expList params){
    if (params == NULL) return NULL;
    return T_ExpList(unEx(params->head), makeTExpList(params->tail));
}

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail)
{
	Tr_expList expList = checked_malloc(sizeof(*expList));
	expList->head = head;
	expList->tail = tail;
	return expList;
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail)
{
	Tr_accessList accessList = checked_malloc(sizeof(*accessList));
	accessList->head = head;
	accessList->tail = tail;
	return accessList;
}

Tr_level Tr_outermost(void){
	static Tr_level outermost = NULL;
	if (outermost == NULL){
		outermost = checked_malloc(sizeof(*outermost));
		Temp_label label = Temp_namedlabel("tigermain");
		outermost->frame = F_newFrame(label, NULL);
		outermost->parent = NULL;
	}
	return outermost;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals){
	Tr_level level = checked_malloc(sizeof(*level));
	U_boolList escapes = U_BoolList(TRUE, formals);
	level->frame = F_newFrame(name, escapes);
	level->parent = parent;
	return level;
}

Tr_accessList Tr_formals(Tr_level level){
	F_accessList f_accessList = F_formals(level->frame)->tail;
	return makeTrAccessList(f_accessList, level);
}

Tr_access Tr_allocLocal(Tr_level level, bool escape){
	F_access f_access = F_allocLocal(level->frame, escape);
	Tr_access t_access = checked_malloc(sizeof(*t_access));
	t_access->access = f_access;
	t_access->level = level;
	return t_access;
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level){
	T_exp access_fp = T_Binop(T_plus, T_Temp(F_FP()), T_Const(0));
	while (access->level != level)
	{
		access_fp = T_Mem(T_Binop(T_plus, access_fp, T_Const(F_wordsize)));
		level = level->parent;
	}
	return ex(F_exp(access->access, access_fp));
}

Tr_exp Tr_fieldVar(Tr_exp addr, int n){
	return ex(T_Mem(T_Binop(T_plus, unEx(addr), T_Const(n * F_wordsize))));
}

Tr_exp Tr_subscriptVar(Tr_exp addr, Tr_exp index){
	return ex(T_Mem(T_Binop(T_plus, unEx(addr), T_Binop(T_mul, T_Const(F_wordsize), unEx(index)))));
}

Tr_exp Tr_nil(){
	return ex(T_Const(0));
}

Tr_exp Tr_int(int value){
	return ex(T_Const(value));
}

Tr_exp Tr_string(string str){
	Temp_label label = Temp_newlabel();
	F_frag frag = F_StringFrag(label, str);

	strfragList = F_FragList(frag, strfragList);
	return ex(T_Name(label));
}

Tr_exp Tr_call(Tr_level caller, Temp_label label, Tr_expList params, Tr_level callee){
	T_exp staticlink = T_Binop(T_plus, T_Temp(F_FP()), T_Const(0));
	T_expList t_params = makeTExpList(params);

	if (callee->parent == NULL){
		return ex(T_Call(T_Name(label), T_ExpList(staticlink, t_params)));
	}
	while (caller != callee->parent){
		staticlink = T_Mem(T_Binop(T_plus, staticlink, T_Const(F_wordsize)));
		caller = caller->parent;
	}

	return ex(T_Call(T_Name(label), T_ExpList(staticlink, t_params)));
}


Tr_exp Tr_arith(A_oper oper, Tr_exp left, Tr_exp right){
	switch (oper){ 
        case A_plusOp: return ex(T_Binop(T_plus, unEx(left), unEx(right)));
        case A_minusOp: return ex(T_Binop(T_minus, unEx(left), unEx(right)));
        case A_timesOp: return ex(T_Binop(T_mul, unEx(left), unEx(right)));
        case A_divideOp: return ex(T_Binop(T_div, unEx(left), unEx(right)));
        default:{
            printf("translate: in arith unexpected oper %d", oper);
            return ex(T_Const(0));
        }
	}
}

Tr_exp Tr_condition(A_oper op, Tr_exp left, Tr_exp right){
	T_stm t_stm;
	switch(op){
		case A_eqOp: t_stm = T_Cjump(T_eq, unEx(left), unEx(right), NULL, NULL); break;
        case A_neqOp: t_stm = T_Cjump(T_ne, unEx(left), unEx(right), NULL, NULL); break;
        case A_ltOp: t_stm = T_Cjump(T_lt, unEx(left), unEx(right), NULL, NULL); break;
        case A_leOp: t_stm = T_Cjump(T_le, unEx(left), unEx(right), NULL, NULL); break;
        case A_gtOp: t_stm = T_Cjump(T_gt, unEx(left), unEx(right), NULL, NULL); break;
        case A_geOp: t_stm = T_Cjump(T_ge, unEx(left), unEx(right), NULL, NULL); break;
        default: t_stm = T_Cjump(T_eq, unEx(left), unEx(right), NULL, NULL);
	}

	struct Cx conditional;
	conditional.stm = t_stm;
    conditional.trues = PatchList(&t_stm->u.CJUMP.true, NULL);
    conditional.falses = PatchList(&t_stm->u.CJUMP.false, NULL);

	return cx(conditional);
}

Tr_exp Tr_strcmp(A_oper oper, Tr_exp left, Tr_exp right){
	T_stm s;
	T_exp ret = F_externalCall("stringEqual", T_ExpList(unEx(left), T_ExpList(unEx(right), NULL)));
	switch (oper){
        case A_eqOp:{
            s = T_Cjump(T_eq, ret, T_Const(1), NULL, NULL);
            break;
        }
        case A_neqOp:{
            s = T_Cjump(T_ne, ret, T_Const(1), NULL, NULL);
            break;
        }
        default: assert(0);
	}
	patchList trues = PatchList(&s->u.CJUMP.true, NULL);
	patchList falses = PatchList(&s->u.CJUMP.false, NULL);

	return Tr_Cx(trues, falses, s);
}

Tr_exp Tr_record(Tr_expList fields){
	int fields_count = 0;

	Tr_expList temp_fields = fields;
	while (temp_fields){
		temp_fields = temp_fields->tail;
		fields_count += 1;
	}

	Temp_temp record_addr = Temp_newtemp();
	T_exp addr = F_externalCall("malloc", T_ExpList(T_Binop(T_mul, T_Const(F_wordsize), T_Const(fields_count)), NULL));

	T_stm s = T_Move(T_Temp(record_addr), addr);

	for (int i = 0; fields; i++, fields = fields->tail){
		T_stm stm = T_Move(T_Mem(T_Binop(T_plus, T_Temp(record_addr), T_Binop(T_mul, T_Const(i), T_Const(F_wordsize)))), unEx(fields->head));
		s = T_Seq(s, stm);
	}

	return ex(T_Eseq(s, T_Temp(record_addr)));
}

Tr_exp Tr_seq(Tr_exp first, Tr_exp second){
	return ex(T_Eseq(unNx(first), unEx(second)));
}

Tr_exp Tr_ifthenelse(Tr_exp test, Tr_exp then, Tr_exp elsee){
	// temp for result
    Temp_temp result = Temp_newtemp();

    // where two branches join
    Temp_label join_label = Temp_newlabel();
    T_exp join_exp = T_Eseq(T_Label(join_label), T_Temp(result));

	// then exps
	Temp_label then_label = Temp_newlabel();
	T_stm then_stm = T_Seq(T_Move(T_Temp(result), unEx(then)),
                           T_Jump(T_Name(join_label), Temp_LabelList(join_label, NULL)));
	then_stm = T_Seq(T_Label(then_label), then_stm);

	// else exps
	Temp_label else_label = Temp_newlabel();
	T_stm else_stm = T_Seq(T_Move(T_Temp(result), unEx(elsee)),
                           T_Jump(T_Name(join_label), Temp_LabelList(join_label, NULL)));
	else_stm = T_Seq(T_Label(else_label), else_stm);

	// build condictional jump
	struct Cx conditional = unCx(test);
	doPatch(conditional.trues, then_label);
	doPatch(conditional.falses, else_label);

	T_exp t_exp = T_Eseq(T_Seq(conditional.stm, T_Seq(then_stm, else_stm)), join_exp);

	return ex(t_exp);
}

Tr_exp Tr_array(Tr_exp size, Tr_exp init){
	Temp_temp array_addr = Temp_newtemp();
	T_stm s = T_Move(T_Temp(array_addr), F_externalCall("initArray", T_ExpList(unEx(size), T_ExpList(unEx(init), NULL))));
	return ex(T_Eseq(s, T_Temp(array_addr)));
}

Tr_exp Tr_assign(Tr_exp lvalue, Tr_exp value){
	return nx(T_Move(unEx(lvalue), unEx(value)));
}

Tr_exp Tr_while(Tr_exp test, Tr_exp body, Temp_label done){
	Temp_label test_label = Temp_newlabel();
	Temp_label body_label = Temp_newlabel();

	struct Cx cx = unCx(test);
	doPatch(cx.trues, body_label);
	doPatch(cx.falses, done);

	T_stm test_stm = T_Seq(T_Label(test_label), cx.stm);
	T_stm body_stm = T_Seq(T_Label(body_label), T_Seq(unNx(body), T_Jump(T_Name(test_label), Temp_LabelList(test_label, NULL))));

	return nx(T_Seq(test_stm, T_Seq(body_stm, T_Label(done))));
}

Tr_exp Tr_for(Tr_access access, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done){
	// init part
	T_exp hi_exp = T_Temp(Temp_newtemp());
	T_exp loop_var = F_exp(access->access, T_Temp(F_FP()));
	T_stm init_stm = T_Seq(T_Move(loop_var, unEx(lo)), T_Move(hi_exp, unEx(hi)));

	// judge part
	Temp_label test_label = Temp_newlabel();
	Temp_label body_label = Temp_newlabel();

	T_stm test_stm = T_Seq(T_Label(test_label),T_Cjump(T_le, loop_var, hi_exp, body_label, done));

	// body part
	T_stm increse_stm = T_Move(loop_var, T_Binop(T_plus, loop_var, T_Const(1)));
	T_stm body_stm = T_Seq(T_Label(body_label),T_Seq(unNx(body),T_Seq(increse_stm,
			T_Jump(T_Name(test_label), Temp_LabelList(test_label, NULL)))));

	return nx(T_Seq(init_stm, T_Seq(test_stm, T_Seq(body_stm, T_Label(done)))));
}

Tr_exp Tr_break(Temp_label done){
	return nx(T_Jump(T_Name(done), Temp_LabelList(done, NULL)));
}

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals){
	T_stm s = T_Move(T_Temp(F_RV()), unEx(body));
	F_frag frag = F_ProcFrag(s, level->frame);
	funcfragList = F_FragList(frag, funcfragList);
}

F_fragList Tr_getResult(void){
	if (strfragList == NULL) return funcfragList;

	F_fragList fl = strfragList;
	while (fl->tail){
		fl = fl->tail;
	}
	fl->tail = funcfragList;
	return strfragList;
}
