#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "env.h"
#include "semant.h"
#include "helper.h"
#include "translate.h"
#include "escape.h"

struct expty{
	Tr_exp exp;
	Ty_ty ty;
};

//In Lab4, the first argument exp should always be **NULL**.
struct expty expTy(Tr_exp exp, Ty_ty ty){
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

Ty_ty base_ty(Ty_ty t){
	// get the built-in root type of t
	while (t && t->kind == Ty_name) {
		t = t->u.name.ty;
	}

	return t;
}

Ty_tyList makeFomalTyList(S_table tenv, A_fieldList fields){
	if (fields == NULL) return NULL;

	Ty_ty head_ty = S_look(tenv, fields->head->typ);
	if (head_ty == NULL){
		EM_error(fields->head->pos, "undefined type %s", S_name(fields->head->typ));
		head_ty = Ty_Int();
	}
	Ty_tyList next = makeFomalTyList(tenv, fields->tail);

	return Ty_TyList(head_ty, next);
}

Ty_fieldList makeRecordTyList(S_table tenv, A_fieldList fields){
	if (fields == NULL) return NULL;

	Ty_ty head_ty = S_look(tenv, fields->head->typ);
	if (head_ty == NULL){
		EM_error(fields->head->pos, "undefined type %s", S_name(fields->head->typ));
		head_ty = Ty_Int();
	}
	Ty_field head_field = Ty_Field(fields->head->name, head_ty);
	Ty_fieldList next = makeRecordTyList(tenv, fields->tail);

	return Ty_FieldList(head_field, next);
}

U_boolList makeBoolList(A_fieldList params){
	if (params == NULL) return NULL;
	return U_BoolList(params->head->escape, makeBoolList(params->tail));
}

Tr_expList reverseTrExpList(Tr_expList el){
	Tr_expList new_el = NULL;
	while (el){
		new_el = Tr_ExpList(el->head, new_el);
		el = el->tail;
	}
	return new_el;
}

struct expty transVar(S_table venv, S_table tenv, A_var v, Tr_level l, Temp_label label){
	switch (v->kind){
        case A_simpleVar:{
            E_enventry x = S_look(venv, v->u.simple);
            if (x && x->kind == E_varEntry){
                struct expty result = expTy(Tr_simpleVar(x->u.var.access, l), base_ty(x->u.var.ty));
                return result;
            }
            else{
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());
            }
        }
        case A_fieldVar:{
            struct expty varty = transVar(venv, tenv, v->u.field.var, l, label);

            if (varty.ty->kind != Ty_record){
                EM_error(v->pos, "not a record type");
                return expTy(NULL, Ty_Int());
            }

            Ty_fieldList record = varty.ty->u.record;
            int index = 0;
            while (record){
                if (record->head->name == v->u.field.sym){
                    return expTy(Tr_fieldVar(varty.exp, index), base_ty(record->head->ty));
                }
                index++;
                record = record->tail;
            }

            EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
            return expTy(NULL, Ty_Int());
        }
        case A_subscriptVar:{
            struct expty varty = transVar(venv, tenv, v->u.subscript.var, l, label);

            if (varty.ty->kind != Ty_array){
                EM_error(v->pos, "array type required");
                return expTy(NULL, Ty_Int());
            }

            struct expty expty = transExp(venv, tenv, v->u.subscript.exp, l, label);

            if (expty.ty->kind != Ty_int){
                EM_error(v->u.subscript.exp->pos, "type should be int");
                return expTy(NULL, Ty_Int());
            }
            return expTy(Tr_subscriptVar(varty.exp, expty.exp), base_ty(varty.ty->u.array));
        }
	}
	assert(0);
}

struct expty transExp(S_table venv, S_table tenv, A_exp a, Tr_level l, Temp_label label){
	switch (a->kind){
        case A_varExp: return transVar(venv, tenv, a->u.var, l, label);
        case A_nilExp: return expTy(Tr_nil(), Ty_Nil());
        case A_intExp: return expTy(Tr_int(a->u.intt), Ty_Int());
        case A_stringExp: return expTy(Tr_string(a->u.stringg), Ty_String());
        case A_breakExp: return expTy(Tr_break(label), Ty_Void());
        case A_callExp:{
            E_enventry x = S_look(venv, a->u.call.func);
            if ((x == NULL) || (x->kind != E_funEntry)){
                EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
                return expTy(Tr_nil(), Ty_Int());
            }

            A_expList args = a->u.call.args;
            Ty_tyList formals = x->u.fun.formals;
            Ty_ty result = x->u.fun.result;
            Tr_expList tr_expList = NULL;

            while (args && formals){
                struct expty arg = transExp(venv, tenv, args->head, l, label);
                Ty_ty formal = formals->head;

                if (base_ty(arg.ty)->kind != base_ty(formal)->kind){
                    EM_error(args->head->pos, "para type mismatch");
                }
                tr_expList = Tr_ExpList(arg.exp, tr_expList);
                args = args->tail;
                formals = formals->tail;
            }

            if (args != NULL){
                EM_error(a->pos, "too many params in function %s", S_name(a->u.call.func));
            }
            if (formals != NULL){
                EM_error(a->pos, "too few params in function %s", S_name(a->u.call.func));
            }
            tr_expList = reverseTrExpList(tr_expList);

            return expTy(Tr_call(l, x->u.fun.label, tr_expList, x->u.fun.level), base_ty(result));
        }
        case A_opExp:{
            A_oper oper = a->u.op.oper;
            struct expty left = transExp(venv, tenv, a->u.op.left, l, label);
            struct expty right = transExp(venv, tenv, a->u.op.right, l, label);
            if (oper == A_plusOp || oper == A_minusOp || oper == A_timesOp || oper == A_divideOp){
                if (base_ty(left.ty)->kind != Ty_int){
                    EM_error(a->u.op.left->pos, "integer required");
                }
                if (base_ty(right.ty)->kind != Ty_int){
                    EM_error(a->u.op.right->pos, "integer required");
                }
                return expTy(Tr_arith(oper, left.exp, right.exp), Ty_Int());
            }
            else if (oper == A_eqOp || oper == A_neqOp){
                if (base_ty(left.ty) != base_ty(right.ty)){
                    if (!((base_ty(left.ty)->kind == Ty_record) &&
                                (base_ty(right.ty)->kind == Ty_nil))){
                        EM_error(a->u.op.left->pos, "same type required");
                    }
                }

                if (base_ty(left.ty)->kind == Ty_string){
                    return expTy(Tr_strcmp(oper, left.exp, right.exp), Ty_Int());
                }
                return expTy(Tr_condition(oper, left.exp, right.exp), Ty_Int());
            }
            else{
                if (base_ty(left.ty) != base_ty(right.ty)){
                    EM_error(a->u.op.left->pos, "same type required");
                }

                return expTy(Tr_condition(oper, left.exp, right.exp), Ty_Int());
            }
        }
        case A_recordExp:{
            Ty_ty recordty = S_look(tenv, a->u.record.typ);
            recordty = base_ty(recordty);

            if ((recordty == NULL) || (recordty->kind != Ty_record)){
                EM_error(a->pos, "undefined type %s", S_name(a->u.record.typ));
                return expTy(NULL, Ty_Int());
            }

            A_efieldList fields = a->u.record.fields;
            Ty_fieldList records = recordty->u.record;
            Tr_expList tr_expList = NULL;
            while (fields && records){
                A_efield efield = fields->head;
                Ty_field field = records->head;

                if (field->name != efield->name){
                    EM_error(a->pos, "type name %s does not match", S_name(efield->name));
                    return expTy(NULL, Ty_Int());
                }

                struct expty actual = transExp(venv, tenv, efield->exp, l, label);
                if (base_ty(actual.ty) != base_ty(field->ty)){
                    if (!((base_ty(field->ty)->kind == Ty_record) &&
                                (base_ty(actual.ty)->kind == Ty_nil))){
                        EM_error(a->pos, "actual type does not match");
                        return expTy(NULL, Ty_Int());
                    }
                }

                tr_expList = Tr_ExpList(actual.exp, tr_expList);
                fields = fields->tail;
                records = records->tail;
            }

            if ((fields != NULL) || (records != NULL)){
                EM_error(a->pos, "type num does not match");
                return expTy(NULL, Ty_Int());
            }
            tr_expList = reverseTrExpList(tr_expList);
            return expTy(Tr_record(tr_expList), recordty);
        }
        case A_seqExp:{
            A_expList seq = a->u.seq;
            struct expty result = expTy(NULL, Ty_Void());
            Tr_exp result_exp = Tr_nil();

            while (seq){
                result = transExp(venv, tenv, seq->head, l, label);
                result_exp = Tr_seq(result_exp, result.exp);
                seq = seq->tail;
            }

            return expTy(result_exp, result.ty);
        }
        case A_assignExp:{
            A_var assign_var = a->u.assign.var;
            A_exp assign_exp = a->u.assign.exp;

            if (assign_var->kind == A_simpleVar){
                E_enventry x = S_look(venv, assign_var->u.simple);
                if (x->readonly){
                    EM_error(assign_var->pos, "loop variable can't be assigned");
                    return expTy(NULL, Ty_Void());
                }
            }

            struct expty assign_exp_ty = transExp(venv, tenv, assign_exp, l, label);
            struct expty assign_var_ty = transVar(venv, tenv, assign_var, l, label);

            if (base_ty(assign_exp_ty.ty) != base_ty(assign_var_ty.ty)){
                if (!((base_ty(assign_var_ty.ty)->kind == Ty_record) &&
                            (base_ty(assign_exp_ty.ty)->kind == Ty_nil))){
                    EM_error(a->pos, "unmatched assign exp");
                    return expTy(NULL, Ty_Void());
                }
            }
            return expTy(Tr_assign(assign_var_ty.exp, assign_exp_ty.exp), Ty_Void());
        }
        case A_ifExp:{
            A_exp test = a->u.iff.test;
            A_exp then = a->u.iff.then;
            A_exp elsee = a->u.iff.elsee;

            struct expty test_ty = transExp(venv, tenv, test, l, label);

            if (base_ty(test_ty.ty)->kind != Ty_int){
                EM_error(test->pos, "if test type error");
                return expTy(NULL, Ty_Int());
            }

            struct expty then_ty = transExp(venv, tenv, then, l, label);
            struct expty elsee_ty = transExp(venv, tenv, elsee, l, label);

            if ((elsee != NULL) && (elsee->kind != A_nilExp)){
                if (base_ty(then_ty.ty) != base_ty(elsee_ty.ty)){
                    EM_error(then->pos, "then exp and else exp type mismatch");
                    return expTy(NULL, Ty_Int());
                }
            }
            else{
                if (base_ty(then_ty.ty)->kind != Ty_void && base_ty(then_ty.ty)->kind != Ty_record){
                    EM_error(then->pos, "if-then exp's body must produce no value %d", base_ty(then_ty.ty)->kind);
                }
            }

            return expTy(Tr_ifthenelse(test_ty.exp, then_ty.exp, elsee_ty.exp), base_ty(then_ty.ty));
        }
        case A_whileExp:{
            A_exp test = a->u.whilee.test;
            A_exp body = a->u.whilee.body;

            Temp_label done = Temp_newlabel();
            struct expty test_ty = transExp(venv, tenv, test, l, label);
            // need a new label
            struct expty body_ty = transExp(venv, tenv, body, l, done);

            if (base_ty(test_ty.ty)->kind != Ty_int) EM_error(test->pos, "while test type should be int");
            if (base_ty(body_ty.ty)->kind != Ty_void) EM_error(body->pos, "while body must produce no value");

            return expTy(Tr_while(test_ty.exp, body_ty.exp, done), Ty_Void());
        }
        case A_forExp:{
            A_exp lo = a->u.forr.lo;
            A_exp hi = a->u.forr.hi;
            A_exp body = a->u.forr.body;
            struct expty lo_ty = transExp(venv, tenv, lo, l, label);
            struct expty hi_ty = transExp(venv, tenv, hi, l, label);

            if ((base_ty(lo_ty.ty)->kind != Ty_int) || (base_ty(hi_ty.ty)->kind != Ty_int)){
                EM_error(lo->pos, "for exp's range type is not integer");
            }
            Temp_label done = Temp_newlabel();

            S_beginScope(venv);
            Tr_access loop_var = Tr_allocLocal(l, a->u.forr.escape);
            S_enter(venv, a->u.forr.var, E_ROVarEntry(loop_var, Ty_Int()));
            // need a new label
            struct expty body_ty = transExp(venv, tenv, body, l, done);
            S_endScope(venv);

            if (base_ty(body_ty.ty)->kind != Ty_void){
                EM_error(body->pos, "for body type should be void");
            }

            return expTy(Tr_for(loop_var, lo_ty.exp, hi_ty.exp, body_ty.exp, done), Ty_Void());
        }
        case A_letExp:{
            struct expty letexp;
            A_decList d;
            Tr_exp tr_exp = Tr_nil();
            S_beginScope(venv);
            S_beginScope(tenv);
            for (d = a->u.let.decs; d; d = d->tail){
                tr_exp = Tr_seq(tr_exp, transDec(venv, tenv, d->head, l, label));
            }
            letexp = transExp(venv, tenv, a->u.let.body, l, label);
            S_endScope(tenv);
            S_endScope(venv);
            return expTy(Tr_seq(tr_exp, letexp.exp), letexp.ty);
        }
        case A_arrayExp:{
            Ty_ty array_ty = S_look(tenv, a->u.array.typ);
            array_ty = base_ty(array_ty);
            if (array_ty == NULL || array_ty->kind != Ty_array){
                EM_error(a->pos, "undefined array %s", S_name(a->u.array.typ));
                return expTy(NULL, Ty_Int());
            }

            A_exp size = a->u.array.size;
            A_exp init = a->u.array.init;

            struct expty size_ty = transExp(venv, tenv, size, l, label);
            struct expty init_ty = transExp(venv, tenv, init, l, label);

            if (base_ty(size_ty.ty)->kind != Ty_int){
                EM_error(size->pos, "array size type should be int");
            }

            if (base_ty(init_ty.ty) != base_ty(array_ty->u.array)){
                EM_error(init->pos, "type mismatch");
            }

            return expTy(Tr_array(size_ty.exp, init_ty.exp), array_ty);
        }
    }
    assert(0);
}

Tr_exp transDec(S_table venv, S_table tenv, A_dec d, Tr_level l, Temp_label label){
    switch (d->kind){
    case A_functionDec:{
        A_fundecList fundeclist = d->u.function;

        // head
        while (fundeclist){
            A_fundec f = fundeclist->head;
            Ty_ty resultTy = Ty_Void();

            A_fundecList dupList = fundeclist->tail;

            while (dupList){
                if (f->name == dupList->head->name){
                    EM_error(f->pos, "two functions have the same name");
                    break;
                }
                dupList = dupList->tail;
            }

            Temp_label fname = f->name;
            Tr_level nlevel = Tr_newLevel(l, fname, makeBoolList(f->params));

            if (f->result != NULL){
                resultTy = S_look(tenv, f->result);
            }

            if (resultTy == NULL){
                EM_error(f->pos, "undefined function return type %s", S_name(f->result));
                resultTy = Ty_Void();
            }

            Ty_tyList formalTys = makeFomalTyList(tenv, f->params);
            S_enter(venv, f->name, E_FunEntry(nlevel, fname, formalTys, resultTy));

            fundeclist = fundeclist->tail;
        }

        // body
        fundeclist = d->u.function;
        while (fundeclist){
            A_fundec f = fundeclist->head;
            A_fieldList fields = f->params;

            E_enventry x = S_look(venv, f->name);
            Tr_accessList formal_accessList = Tr_formals(x->u.fun.level);
            Ty_tyList formalTys = makeFomalTyList(tenv, f->params);
            S_beginScope(venv);
            while (fields != NULL && formalTys != NULL && formal_accessList != NULL){
                Ty_ty formal = formalTys->head;
                A_field field = fields->head;
                S_enter(venv, field->name, E_VarEntry(formal_accessList->head, formal));

                fields = fields->tail;
                formalTys = formalTys->tail;
                formal_accessList = formal_accessList->tail;
            }

            struct expty e = transExp(venv, tenv, f->body, x->u.fun.level, label);

            Ty_ty resultTy = x->u.fun.result;

            if (base_ty(resultTy) != base_ty(e.ty)){
                if (base_ty(resultTy)->kind == Ty_void){
                    EM_error(f->pos, "procedure returns value");
                }
                else{
                    if (!(base_ty(resultTy)->kind == Ty_record && base_ty(e.ty)->kind == Ty_nil)){
                        EM_error(f->pos, "function return type does not match");
                    }
                }
            }
            S_endScope(venv);

            Tr_procEntryExit(x->u.fun.level, e.exp, formal_accessList);
            fundeclist = fundeclist->tail;
        }

        return Tr_nil();
    }
    case A_varDec:{
        struct expty e = transExp(venv, tenv, d->u.var.init, l, label);
        Tr_access tr_access;
        if (d->u.var.typ != NULL){
            Ty_ty var_ty = S_look(tenv, d->u.var.typ);

            if (var_ty == NULL){
                EM_error(d->pos, "undefined type %s", S_name(d->u.var.typ));
                return Tr_nil();
            }

            if (base_ty(var_ty)->kind == Ty_record){
                if (base_ty(e.ty) == base_ty(var_ty) || base_ty(e.ty)->kind == Ty_nil){
                    tr_access = Tr_allocLocal(l, d->u.var.escape);
                    S_enter(venv, d->u.var.var, E_VarEntry(tr_access, var_ty));
                    return Tr_assign(Tr_simpleVar(tr_access, l), e.exp);
                }

                EM_error(d->pos, "type mismatch");
            }
            else{
                if (base_ty(var_ty) == base_ty(e.ty)){
                    tr_access = Tr_allocLocal(l, d->u.var.escape);
                    S_enter(venv, d->u.var.var, E_VarEntry(tr_access, var_ty));
                    return Tr_assign(Tr_simpleVar(tr_access, l), e.exp);
                }

                EM_error(d->pos, "type mismatch");
            }
        }
        else{
            if (base_ty(e.ty)->kind == Ty_nil){
                EM_error(d->u.var.init->pos, "init should not be nil without type specified");
            }
        }
        tr_access = Tr_allocLocal(l, d->u.var.escape);
        S_enter(venv, d->u.var.var, E_VarEntry(tr_access, e.ty));

        return Tr_assign(Tr_simpleVar(tr_access, l), e.exp);
    }
    case A_typeDec:{
        A_nametyList typelist = d->u.type;

        // head
        while (typelist){
            A_namety t = typelist->head;
            A_nametyList dupList = typelist->tail;

            while (dupList){
                if (t->name == dupList->head->name){
                    EM_error(d->pos, "two types have the same name");
                    break;
                }
                dupList = dupList->tail;
            }

            S_enter(tenv, t->name, Ty_Name(t->name, NULL));
            typelist = typelist->tail;
        }

        // main body part
        typelist = d->u.type;
        while (typelist){
            A_namety t = typelist->head;
            Ty_ty ty = S_look(tenv, t->name);
            ty->u.name.ty = transTy(tenv, t->ty);

            typelist = typelist->tail;
        }

        // check whether there is a cycle
        typelist = d->u.type;
        while (typelist){
            A_namety n = typelist->head;
            Ty_ty i = S_look(tenv, n->name);
            Ty_ty t = i;
            t = t->u.name.ty;

            while (t->kind == Ty_name){
                if (t->u.name.sym == n->name)
                {
                    EM_error(d->pos, "illegal type cycle");
                    i->u.name.ty = Ty_Void();
                    break;
                }
                t = t->u.name.ty;
            }

            typelist = typelist->tail;
        }
        return Tr_nil();
        }
	}
}

Ty_ty transTy(S_table tenv, A_ty a){
	switch (a->kind){
        case A_recordTy: return Ty_Record(makeRecordTyList(tenv, a->u.record));
        case A_nameTy:{
            Ty_ty t = S_look(tenv, a->u.name);
            if (t == NULL){
                EM_error(a->pos, "undefined type %s", S_name(a->u.name));
                return Ty_Int();
            }
            return Ty_Name(a->u.name, t);
        }
        case A_arrayTy:{
            Ty_ty t = S_look(tenv, a->u.array);
            if (t == NULL){
                EM_error(a->pos, "undefined type %s", S_name(a->u.array));
                return Ty_Int();
            }
            return Ty_Array(t);
        }
	}
	assert(0);
}

F_fragList SEM_transProg(A_exp exp){
	S_table tenv = E_base_tenv();
	S_table venv = E_base_venv();
    Tr_level main_level = Tr_outermost();
	Temp_label main_label = Temp_newlabel();

	struct expty result = transExp(venv, tenv, exp, main_level, main_label);

	Tr_procEntryExit(main_level, result.exp, NULL);
	return Tr_getResult();
}
