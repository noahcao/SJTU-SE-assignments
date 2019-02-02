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

//LAB5: you can modify anything you want.
static int wordsize = 8;
// maintain the global variable for frags
static struct F_fragList_ dummy = {NULL, NULL};
static F_fragList frag_head = &dummy, frag_tail = &dummy;

// ----------- define constructors -------------
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
    Tr_accessList list = checked_malloc(sizeof(*list));
    list->head = head;
    list->tail = tail;
    return list;
}

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail) {
    Tr_expList list = checked_malloc(sizeof(*list));
    list->head = head;
    list->tail = tail;
    return list;
}

// ----------------------------------------------
static patchList PatchList(Temp_label *head, patchList tail)
{
	patchList list;

	list = (patchList)checked_malloc(sizeof(struct patchList_));
	list->head = head;
	list->tail = tail;
	return list;
}

void doPatch(patchList list, Temp_label label){
	for(; list; list = list->tail){
		*(list->head) = label;
	}
}

patchList joinPatch(patchList first, patchList second)
{
	if(!first) return second;
	for(; first->tail; first = first->tail);
	first->tail = second;
	return first;
}

// construct three types of exps
static Tr_exp ex(T_exp exp) {
    Tr_exp tr_exp = checked_malloc(sizeof(*tr_exp));
    tr_exp->kind = Tr_ex;
    tr_exp->u.ex = exp;
    return tr_exp;
}

static Tr_exp nx(T_stm stm) {
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
	// transform exp of any type to a Cx
	switch(e->kind){
		case Tr_cx: return e->u.cx;
		case Tr_nx: assert(0); // cannot transform nx to cx
		case Tr_ex:{
			if(e->u.ex->kind == T_CONST){
				// constant case should be optimized as demanded in textbook
				if(e->u.ex->u.CONST == 0){
					struct Cx target;
					target.stm = T_Jump(T_Name(NULL), Temp_LabelList(NULL, NULL));
					target.trues = PatchList(&target.stm->u.JUMP.exp->u.NAME, PatchList(&target.stm->u.JUMP.jumps->head, NULL));
					target.falses = NULL;
					return target;
				}
				else{
					struct Cx target;
					target.stm = T_Jump(T_Name(NULL), Temp_LabelList(NULL, NULL));
					target.falses = PatchList(&target.stm->u.JUMP.exp->u.NAME, PatchList(&target.stm->u.JUMP.jumps->head, NULL));
					target.trues = NULL;
					return target;
				}
			}

			// not a T_CONST
			struct Cx conditional_exp;
			conditional_exp.stm = T_Cjump(T_ne, e->u.ex, T_Const(0), NULL, NULL);
			conditional_exp.trues = PatchList(&conditional_exp.stm->u.CJUMP.true, NULL);
			conditional_exp.falses = PatchList(&conditional_exp.stm->u.CJUMP.true, NULL);
			return conditional_exp;
		}
	}
}

Tr_level Tr_outermost(void) {
    static struct Tr_level_ outermost;
    outermost.frame = NULL;
    outermost.parent = NULL;
    return &outermost;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals){
	Tr_level level = checked_malloc(sizeof(*level));
	U_boolList escapes = U_BoolList(TRUE, formals);
	level->frame = F_newFrame(name, escapes);
	level->parent = parent;
	return level;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape){
	F_access f_access = F_allocLocal(level->frame, escape);
	Tr_access t_access = checked_malloc(sizeof(*t_access));
	t_access->access = f_access;
	t_access->level = level;
	return t_access;
}

static Tr_accessList makeTrAccessList(F_accessList list, Tr_level level){
	if(list == NULL) return NULL;

	Tr_access t_access = checked_malloc(sizeof(*t_access));
	t_access->level = level;
	t_access->access = list->head;

	return Tr_AccessList(t_access, makeTrAccessList(list->tail, level));
}

Tr_accessList Tr_formals(Tr_level level){
	F_accessList f_accessList = F_formals(level->frame)->tail;
	return makeTrAccessList(f_accessList, level);
}

static void Tr_addFrag(F_frag frag) {
    frag_tail->tail = F_FragList(frag, NULL);
    frag_tail = frag_tail->tail;
}

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals){
	T_stm t_stm = T_Move(T_Temp(F_RV()), unEx(body));
	F_frag frag = F_ProcFrag(t_stm, level->frame);
	Tr_addFrag(frag);
}

F_fragList Tr_getResult(void){
	return frag_head->tail;
}

// expression functions
Tr_exp Tr_simpleVar(Tr_access access, Tr_level level){
	T_exp frame = T_Temp(F_FP());
	while(level != access->level){
		frame = T_Mem(T_Binop(T_plus, frame, T_Const(wordsize * 2)));
		level = level->parent;
	}

	T_exp exp = F_exp(access->access, frame);
	return ex(exp);
}

Tr_exp Tr_fieldVar(Tr_exp ptr, int index){
	T_exp exp = T_Mem(T_Binop(T_plus, unEx(ptr), T_Const(index * wordsize)));
	return ex(exp);
}

Tr_exp Tr_arrayVar(Tr_exp ptr, Tr_exp index){
	if(ptr->kind != Tr_ex)  printf("invalid var ptr of an array\n");

	T_exp exp = T_Mem(T_Binop(T_plus, unEx(ptr), T_Binop(T_mul, T_Const(wordsize), unEx(index))));
	return ex(exp);
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
	Tr_addFrag(frag);
	return ex(T_Name(label));
}

static T_expList toTExpList(Tr_expList tr_expList) {
    if (tr_expList == NULL)
        return NULL;
    return T_ExpList(tr_expList->head->u.ex, toTExpList(tr_expList->tail));
}

Tr_exp Tr_call(Temp_label fun, Tr_expList args, Tr_level caller, Tr_level callee){
	// current pointer to the frame is the static link
	T_exp static_link = T_Temp(F_FP());

	while(caller != callee->parent){
		static_link = T_Mem(T_Binop(T_plus, static_link, T_Const(wordsize * 2)));
		caller = caller->parent;
	}

	T_expList t_exp_list = toTExpList(args);
	// add the static link to the exp_list
	t_exp_list = T_ExpList(static_link, t_exp_list);

	return ex(T_Call(T_Name(fun), t_exp_list));
}


Tr_exp Tr_arithmetic(A_oper oper, Tr_exp left, Tr_exp right) {
    T_exp t_exp;
    switch (oper) {
        case A_plusOp:
            t_exp = T_Binop(T_plus, unEx(left), unEx(right));
            break;
        case A_minusOp:
            t_exp = T_Binop(T_minus, unEx(left), unEx(right));
            break;
        case A_timesOp:
            t_exp = T_Binop(T_mul, unEx(left), unEx(right));
            break;
        case A_divideOp:
            t_exp = T_Binop(T_div, unEx(left), unEx(right));
            break;
        default:
            //printf("Translate: unexpected arithmetic operator %d", oper);
            t_exp = T_Const(0);
    }

    return ex(t_exp);
}

Tr_exp Tr_recordCreation(Tr_expList fields){
	Temp_temp record_ptr = Temp_newtemp();

	// count the number of field for further memory malloc
	int field_number = 0;
	Tr_expList search_head = fields;
	while(search_head){
		field_number += 1;
		search_head = search_head->tail;
	}

	// call external malloc for heap memory
	T_exp heap_malloc = F_externalCall("malloc",
                                       T_ExpList(T_Binop(T_mul, T_Const(wordsize), T_Const(field_number)), NULL));

	T_Move(T_Temp(record_ptr), heap_malloc);

	// initilalize the fields
	T_exp t_exp = T_Temp(record_ptr);
	for(int i = 0; i < field_number; i++){
		T_exp dst = T_Binop(T_plus, T_Temp(record_ptr), T_Binop(T_mul, T_Const(wordsize), T_Const(i)));;
		T_stm stm = T_Move(dst, unEx(fields->head));
		t_exp = T_Eseq(stm, t_exp);
		fields = fields->tail;
	}

	return ex(t_exp);
}

Tr_exp Tr_eseq(Tr_exp first, Tr_exp second){
	//printf(" --- Tr_eseq\n");
	T_stm first_stm = unNx(first);
	//printf(" finish first one\n");
	T_exp second_exp = unEx(second);
	//printf(" finish second one\n");
	Tr_exp res = ex(T_Eseq(first_stm, second_exp));
	//printf(" --- Tr_eseq end\n");
	return res;
}

Tr_exp Tr_ifthen(Tr_exp test, Tr_exp then){
	// end-label for false case
	Temp_label end_label = Temp_newlabel();

	// true case: for exps
    Temp_label then_label = Temp_newlabel();
    T_stm then_stm = T_Seq(T_Label(then_label), unNx(then));

	struct Cx conditional = unCx(test);
    doPatch(conditional.trues, then_label);
    doPatch(conditional.falses, end_label);

	T_stm t_seq = T_Seq(conditional.stm, T_Seq(then_stm, T_Label(end_label)));

    return nx(t_seq);
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

Tr_exp Tr_arrayCreation(Tr_exp size, Tr_exp init_value){
	T_exp t_exp = F_externalCall("initArray", T_ExpList(unEx(size), T_ExpList(unEx(init_value), NULL)));

	return ex(t_exp);
}

Tr_exp Tr_condition(A_oper op, Tr_exp left, Tr_exp right){
	T_stm t_stm;
	switch(op){
		case A_eqOp:
            t_stm = T_Cjump(T_eq, unEx(left), unEx(right), NULL, NULL);
            break;
        case A_neqOp:
            t_stm = T_Cjump(T_ne, unEx(left), unEx(right), NULL, NULL);
            break;
        case A_ltOp:
            t_stm = T_Cjump(T_lt, unEx(left), unEx(right), NULL, NULL);
            break;
        case A_leOp:
            t_stm = T_Cjump(T_le, unEx(left), unEx(right), NULL, NULL);
            break;
        case A_gtOp:
            t_stm = T_Cjump(T_gt, unEx(left), unEx(right), NULL, NULL);
            break;
        case A_geOp:
            t_stm = T_Cjump(T_ge, unEx(left), unEx(right), NULL, NULL);
            break;
        default:
            //printf("Translate: unexpected condition operator %d", op);
			t_stm = T_Cjump(T_eq, unEx(left), unEx(right), NULL, NULL);
	}

	struct Cx conditional;
	conditional.stm = t_stm;
    conditional.trues = PatchList(&t_stm->u.CJUMP.true, NULL);
    conditional.falses = PatchList(&t_stm->u.CJUMP.false, NULL);

	return cx(conditional);
}

Tr_exp Tr_assign(Tr_exp lvalue, Tr_exp value){
	//printf(" ---- Tr_assign\n");
	Tr_exp res = nx(T_Move(unEx(lvalue), unEx(value)));
	//printf(" ---- Tr_assign end\n");
	return res;
}

Tr_exp Tr_while(Tr_exp test, Tr_exp body, Temp_label done){
	//printf(" ---- Tr_while\n");
	Temp_label test_label = Temp_newlabel();
	Temp_label body_label = Temp_newlabel();

	struct Cx conditional = unCx(test);

	doPatch(conditional.trues, body_label);
	doPatch(conditional.falses, done);

	T_stm test_stm = T_Seq(T_Label(test_label), conditional.stm);
	T_stm body_stm = T_Seq(T_Label(body_label), T_Seq(unNx(body), T_Jump(T_Name(test_label), Temp_LabelList(test_label, NULL))));

	return nx(T_Seq(test_stm, T_Seq(body_stm, T_Label(done))));
}

Tr_exp Tr_for(Tr_access loop_control_access, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done_label){
	// init part
	T_exp limit = T_Temp(Temp_newtemp());
	T_exp loop_var = F_exp(loop_control_access->access, T_Temp(F_FP()));
	T_stm init_stm = T_Seq(T_Move(loop_var, unEx(lo)), T_Move(limit, unEx(hi)));

	// judge part
	Temp_label if_label = Temp_newlabel();
	T_stm if_stm = T_Seq(T_Cjump(T_le, loop_var, limit, if_label, done_label), T_Seq(T_Label(if_label), unNx(body)));

	// body part
	Temp_label test_label =  Temp_newlabel();
	Temp_label body_label = Temp_newlabel();
	T_stm condition_stm = T_Seq(T_Label(test_label), T_Cjump(T_lt, loop_var, limit, body_label, done_label));
	T_stm increment_stm = T_Move(loop_var, T_Binop(T_plus, loop_var, T_Const(1)));
	T_stm body_stm = T_Seq(T_Label(body_label), T_Seq(increment_stm, T_Seq(unNx(body), T_Jump(T_Name(test_label), Temp_LabelList(test_label, NULL)))));

	body_stm = T_Seq(condition_stm, body_stm);

	return nx(T_Seq(init_stm, T_Seq(if_stm, body_stm)));
}

Tr_exp Tr_break(Temp_label done_label) {
    return nx(T_Jump(T_Name(done_label), Temp_LabelList(done_label, NULL)));
}