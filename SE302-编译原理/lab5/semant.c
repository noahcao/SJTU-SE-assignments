#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "helper.h"
#include "env.h"
#include "semant.h"

/*Lab4: Your implementation of lab4*/

//typedef void* Tr_exp;
struct expty 
{
	Tr_exp exp; 
	Ty_ty ty;
};

//In Lab4, the first argument exp should always be **NULL**.
struct expty expTy(Tr_exp exp, Ty_ty ty)
{
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

F_fragList  SEM_transProg(A_exp exp) {
	Temp_label f_label = Temp_newlabel();
	Tr_level main_level = Tr_newLevel(Tr_outermost(), f_label, NULL);
	E_enventry f_entry = E_FunEntry(main_level, f_label, NULL, Ty_Void()); // main returns void
	struct expty main_body = transExp(E_base_venv(), E_base_tenv(), exp, main_level, NULL);
	Tr_procEntryExit(f_entry->u.fun.level, main_body.exp, NULL);

	return Tr_getResult();
}

Ty_ty base_ty(Ty_ty t) {
	// get the built-in root type of t
	while (t && t->kind == Ty_name) {
		t = t->u.name.ty;
	}

	return t;
}

int hasLoopRef(S_table venv, A_var v){
	// judge if there is a loop reference for variable 
	switch(v->kind){
		case A_simpleVar: {
			E_enventry x = S_look(venv, v->u.simple);
			if(x->readonly){
				return 1;
			}
			return 0;
		}
		case A_fieldVar: return hasLoopRef(venv, v->u.field.var);
		case A_subscriptVar: return hasLoopRef(venv, v->u.subscript.var);
	}
}

Ty_tyList formalTyList(S_table tenv, A_fieldList params){
	if(!params) return NULL;	

	Ty_ty type = S_look(tenv, params->head->typ);
	return Ty_TyList(type, formalTyList(tenv, params->tail));
}

Ty_fieldList fieldList(S_table tenv, A_fieldList fields) {
	Ty_ty type = S_look(tenv, fields->head->typ);
	if (!type) {
		EM_error(fields->head->pos, "undefined type %s", S_name(fields->head->typ));
		type = Ty_Int();
	}
	Ty_field field = Ty_Field(fields->head->name, type);

	if (fields->tail == NULL)  return Ty_FieldList(field, NULL); 
	else  return Ty_FieldList(field, fieldList(tenv, fields->tail));
}

Ty_ty transTy (S_table tenv, A_ty a) {
	switch (a->kind) {
		case A_nameTy: {
			Ty_ty type = S_look(tenv, a->u.name);
			if (!type) {
				EM_error(a->pos, "undefined type %s", S_name(a->u.name));
				return Ty_Int();
			}

			return Ty_Name(a->u.name, type);
		}
		case A_recordTy:
			return Ty_Record(fieldList(tenv, a->u.record));
		case A_arrayTy: {
			Ty_ty type = S_look(tenv, a->u.array);
			if (!type) {
				EM_error(a->pos, "undefined type %s", S_name(a->u.array));
				return Ty_Int();
			}

			return Ty_Array(type);
		}
	}

	return NULL;
}

struct expty transExp(S_table venv, S_table tenv, A_exp a, Tr_level level, Temp_label nearest_break){
	//printf("transExp\n");
	switch(a->kind){
		case A_varExp:{
			return transVar(venv, tenv, a->u.var, level, nearest_break);
		}
		case A_nilExp:{
			return expTy(Tr_nil(), Ty_Nil());
		}
		case A_intExp:{
			return expTy(Tr_int(a->u.intt), Ty_Int());
		}
		case A_stringExp:{
			return expTy(Tr_string(a->u.stringg), Ty_String());
		}
		case A_breakExp:{
			return expTy(Tr_break(nearest_break), Ty_Void());
		}
		case A_opExp: {
			A_oper oper = a->u.op.oper;
			struct expty left = transExp(venv, tenv, a->u.op.left, level, nearest_break);
			struct expty right = transExp(venv, tenv, a->u.op.right, level, nearest_break);

			if(oper == A_plusOp || oper == A_minusOp || oper == A_timesOp || oper == A_divideOp){
				if(left.ty->kind != Ty_int){
					EM_error(a->u.op.left->pos, "integer required");
					return expTy(NULL, Ty_Int());
				}
				if(right.ty->kind != Ty_int){
					EM_error(a->u.op.right->pos, "integer required");
					return expTy(NULL, Ty_Int());
				}
				// legal 
				Tr_exp tr_exp = Tr_arithmetic(oper, left.exp, right.exp);
				return expTy(tr_exp, Ty_Int());	
			}
			else{
				Ty_ty left_ty = base_ty(left.ty);
                Ty_ty right_ty = base_ty(right.ty);
				if(left_ty != right_ty && !(left_ty->kind == Ty_record && right_ty->kind == Ty_nil)){
					EM_error(a->u.op.left->pos, "same type required");
				}
			}
			Tr_exp tr_exp = Tr_condition(oper, left.exp, right.exp);
			return expTy(tr_exp, Ty_Int());
		}
		case A_letExp: {
			struct expty exp;
			A_decList d;
			S_beginScope(venv);
			S_beginScope(tenv);
			for (d = a->u.let.decs; d; d = d->tail) {
				transDec(venv, tenv, d->head, level, nearest_break);
			}
			exp = transExp(venv, tenv, a->u.let.body, level, nearest_break);
			S_endScope(tenv);
			S_endScope(venv);
			return exp;
		}
		case A_arrayExp: {
			Ty_ty type = S_look(tenv, a->u.array.typ);
			type = base_ty(type);

			if(!type){
				EM_error(a->pos, "undefined type %s", S_name(a->u.array.typ));
				return expTy(NULL, Ty_Int());
			}

			if(type->kind != Ty_array){
				EM_error(a->pos, "not array type %s", S_name(a->u.array.typ));
				return expTy(NULL, type);
			}

			struct expty size = transExp(venv, tenv, a->u.array.size, level, nearest_break);
			struct expty init = transExp(venv, tenv, a->u.array.init, level, nearest_break);

			if(base_ty(size.ty)->kind != Ty_int){
				EM_error(a->u.array.size->pos, "type of size expression should be int");
			}
			if (base_ty(init.ty) != base_ty(type->u.array)){
				EM_error(a->u.array.init->pos, "type mismatch");
			}
			
			Tr_exp tr_exp = Tr_arrayCreation(size.exp, init.exp);
			return expTy(tr_exp, type);
		}
		case A_callExp: {
			E_enventry x = S_look(venv, a->u.call.func);
			if(!x || x->kind != E_funEntry){
				EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
				return expTy(NULL, Ty_Int());
			}

			Ty_tyList expected = x->u.fun.formals;
			A_expList parms = a->u.call.args;

			Tr_expList list = Tr_ExpList(NULL, NULL);  // dummy head node
            Tr_expList tailList = list;

			// check whether input params match the expected types
			for(; parms && expected; parms = parms->tail, expected = expected->tail){
				struct expty exp =transExp(venv, tenv, parms->head, level, nearest_break);

				Ty_ty expcted_type = base_ty(expected->head);
				Ty_ty actual_type = base_ty(exp.ty);
				if(base_ty(exp.ty)->kind != base_ty(expected->head)->kind){
					EM_error(parms->head->pos, "para type mismatch");
				}

				tailList->tail = Tr_ExpList(exp.exp, NULL);
				tailList = tailList->tail;
			}

			if(expected != NULL || parms != NULL){
				// here should no paramerters left
				EM_error(a->pos, "too many params in function %s", S_name(a->u.call.func));
			}

			list = list->tail;
			Tr_exp tr_exp = Tr_call(x->u.fun.label, list, level, x->u.fun.level);

			return expTy(tr_exp,  base_ty(x->u.fun.result));
		}
		case A_recordExp: {
			Ty_ty type = S_look(tenv, a->u.record.typ);
			type = base_ty(type);

			if(!type){
				EM_error(a->pos, "undefined type %s", S_name(a->u.record.typ));
				return expTy(NULL, Ty_Int());
			}

			if(type->kind != Ty_record){
				EM_error(a->pos, "not record type %s", S_name(a->u.record.typ));
				return expTy(NULL, type);
			}

			Ty_fieldList expected = type->u.record;
			A_efieldList params = a->u.record.fields;

			Tr_expList list = Tr_ExpList(NULL, NULL);  // dummy head node
            Tr_expList tail = list;

			for(; params && expected; params = params->tail, expected = expected->tail){
				// check the fileds name
				if(expected->head->name != params->head->name){
					EM_error(a->pos, "expected %s but get %s", S_name(expected->head->name), S_name(params->head->name));
				}
				struct expty exp = transExp(venv, tenv, params->head->exp, level, nearest_break);

				Ty_ty expected_type = base_ty(expected->head->ty);
                Ty_ty actual_type = base_ty(exp.ty);
				if(expected_type != actual_type && !(expected_type->kind == Ty_record && actual_type->kind == Ty_nil)) 
					EM_error(a->pos, "type not match");

				tail->tail = Tr_ExpList(exp.exp, NULL);
				tail = tail->tail;
			}

			if(expected != NULL || params != NULL){
				EM_error(a->pos, "field number of %s does not match", S_name(a->u.record.typ));
			}

			list = list->tail;

			Tr_exp tr_exp = Tr_recordCreation(list);

			return expTy(tr_exp, type);
		}
		case A_seqExp: {
			A_expList seq = a->u.seq;
			Tr_exp result_exp = Tr_nil();
            Ty_ty result_type = Ty_Void();
			struct expty exp = expTy(NULL, Ty_Void());
			while(seq){
				exp = transExp(venv, tenv, seq->head, level, nearest_break);
				result_exp = Tr_eseq(result_exp, exp.exp);
				result_type = exp.ty;
				seq = seq->tail;
			}
			return expTy(result_exp, result_type);
		}
		case A_assignExp: {
			struct expty lvalue = transVar(venv, tenv, a->u.assign.var, level, nearest_break);
			struct expty exp = transExp(venv, tenv, a->u.assign.exp, level, nearest_break);

			int hasLoop = hasLoopRef(venv, a->u.assign.var);
			if(hasLoop == 1){
				EM_error(a->pos, "loop variable can't be assigned");
			}
			
			if(base_ty(lvalue.ty) != base_ty(exp.ty)){
				EM_error(a->pos, "unmatched assign exp");
			}

			Tr_exp tr_exp = Tr_assign(lvalue.exp, exp.exp);
			return expTy(tr_exp, Ty_Void());
		}
		case A_ifExp: {
			struct expty test = transExp(venv, tenv, a->u.iff.test, level, nearest_break);
			if(base_ty(test.ty)->kind != Ty_int){
				EM_error(a->u.iff.test->pos, "type of test expression shoulf be int");
			}

			struct expty then = transExp(venv, tenv, a->u.iff.then, level, nearest_break);
			struct expty elseexp;

			Tr_exp tr_exp;
			if(a->u.iff.elsee){
				elseexp = transExp(venv, tenv, a->u.iff.elsee, level, nearest_break);

				Ty_ty then_type = base_ty(then.ty);
                Ty_ty else_type = base_ty(elseexp.ty);

				if(then_type != else_type && !(then_type->kind == Ty_record && else_type->kind == Ty_nil)){
					//EM_error(a->u.iff.then->pos, "if-then exp's body must produce no value");
					EM_error(a->u.iff.then->pos, "then exp and else exp type mismatch");
				}

				tr_exp = Tr_ifthenelse(test.exp, then.exp, elseexp.exp);
			}
			else{
				elseexp = expTy(NULL, Ty_Void());
				if(base_ty(then.ty)->kind != Ty_void || base_ty(elseexp.ty)->kind != Ty_void){
					EM_error(a->u.iff.then->pos, "if-then exp's body must produce no value");
				}

				tr_exp = Tr_ifthen(test.exp, then.exp);
			}

			return expTy(tr_exp, then.ty);
		}
		case A_whileExp: {
			Temp_label new_break = Temp_newlabel();

			struct expty test = transExp(venv, tenv, a->u.whilee.test, level, nearest_break);
			struct expty body = transExp(venv, tenv, a->u.whilee.body, level, new_break);

			if(base_ty(test.ty)->kind != Ty_int){
				EM_error(a->u.whilee.test->pos, "type of test expression shoulf be int");
			}

			if(base_ty(body.ty)->kind != Ty_void){
				EM_error(a->u.whilee.body->pos, "while body must produce no value");
			}

			Tr_exp tr_exp = Tr_while(test.exp, body.exp, new_break);
			return expTy(tr_exp, Ty_Void());
		}
		case A_forExp: {
			struct expty start = transExp(venv, tenv, a->u.forr.lo, level, nearest_break);
			struct expty end = transExp(venv, tenv, a->u.forr.hi, level, nearest_break);

			if(base_ty(start.ty)->kind != Ty_int){
				EM_error(a->u.forr.lo->pos, "for exp's range type is not integer");
			}
			if(base_ty(end.ty)->kind != Ty_int){
				EM_error(a->u.forr.hi->pos, "for exp's range type is not integer");
			}

			S_beginScope(venv);

			Tr_access access = Tr_allocLocal(level, FALSE);
			S_enter(venv, a->u.forr.var, E_ROVarEntry(access, Ty_Int()));

			Temp_label new_break = Temp_newlabel();
			struct expty body = transExp(venv, tenv, a->u.forr.body, level, new_break);
			S_endScope(venv);
			if(base_ty(body.ty)->kind != Ty_void){
				EM_error(a->u.forr.body->pos, "type of body expression should be void");
			}

			Tr_exp tr_exp = Tr_for(access, start.exp, end.exp, body.exp, new_break);

			return expTy(tr_exp, Ty_Void());
		}
	}
}

struct expty transVar(S_table venv, S_table tenv, A_var v, Tr_level level, Temp_label nearest_break){
	switch(v->kind){
		case A_simpleVar: {
			E_enventry x = S_look(venv, v->u.simple);
			if (!x || x->kind != E_varEntry){
				EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
				return expTy(NULL, Ty_Int());
			}
			else{
				Tr_exp tr_exp = Tr_simpleVar(x->u.var.access, level);
				return expTy(tr_exp, base_ty(x->u.var.ty));
			}
		}
		case A_fieldVar: {
			struct expty value = transVar(venv, tenv, v->u.field.var, level, nearest_break);

			if(value.ty->kind != Ty_record){
				EM_error(v->pos, "not a record type");
				return expTy(NULL, Ty_Int());
			}

			Ty_fieldList fileds = value.ty->u.record;
			int field_number = 0;
			while(fileds && fileds->head->name != v->u.field.sym){
				field_number += 1;
				fileds = fileds->tail;
			}
			if(fileds == NULL){
				EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
				return expTy(NULL, Ty_Int());
			}

			Tr_exp tr_exp = Tr_fieldVar(value.exp, field_number);
			return expTy(tr_exp, base_ty(fileds->head->ty));
		}
		case A_subscriptVar: {
			struct expty value = transVar(venv, tenv, v->u.subscript.var, level, nearest_break);
			struct expty index = transExp(venv, tenv, v->u.subscript.exp, level, nearest_break);

			if(value.ty->kind != Ty_array){
				EM_error(v->pos, "array type required");
				return expTy(NULL, Ty_Int());
			}

			if (index.ty->kind != Ty_int) {
				EM_error(v->pos, "index type is not int");
				return expTy(NULL, Ty_Int());
			}

			Tr_exp tr_exp = Tr_arrayVar(value.exp, index.exp);
			return expTy(tr_exp, base_ty(value.ty->u.array));
		}
	}
}

U_boolList makeEscapeList(A_fieldList params) {
    if (params == NULL) {
        return NULL;
    }

    return U_BoolList(params->head->escape, makeEscapeList(params->tail));
}


Tr_exp transDec(S_table venv, S_table tenv, A_dec d, Tr_level level, Temp_label nearest_break){
	switch(d->kind){
		case A_varDec: {
			struct expty e = transExp(venv, tenv, d->u.var.init, level, nearest_break);
			if(d->u.var.typ){
				Ty_ty type = S_look(tenv, d->u.var.typ);
				if (!type){
					EM_error(d->u.var.init->pos, "type not exist %s", S_name(d->u.var.typ));
				}

				Ty_ty expected = base_ty(type);
                Ty_ty actual = base_ty(e.ty);
				if(expected != actual && !(expected->kind == Ty_record && actual->kind == Ty_nil)){
					EM_error(d->u.var.init->pos, "type mismatch");
				}
			}else{
				if (base_ty(e.ty)->kind == Ty_nil){
					EM_error(d->u.var.init->pos, "init should not be nil without type specified");
				}
			}
			S_enter(venv, d->u.var.var, E_VarEntry(Tr_allocLocal(level, FALSE), e.ty));
			break;
		}
		case A_typeDec: {
			A_nametyList types = d->u.type;
			// step 1: add type declralations into the env
			while(types){
				//if (S_look(tenv, types->head->name) != NULL) 
				//	EM_error(d->pos, "two types have the same name");
				//else 
				S_enter(tenv, types->head->name, Ty_Name(types->head->name, NULL));
				types = types->tail;
			}

			// step 2: resolve references occurs
			types = d->u.type;
			while(types){
				Ty_ty type = S_look(tenv, types->head->name);
				type->u.name.ty = transTy(tenv, types->head->ty);
				types = types->tail;
			}

			// step 3: solver the cycle reference
			types = d->u.type;
			while(types){
				Ty_ty init = S_look(tenv, types->head->name);
				Ty_ty type = init;
				while((type = type->u.name.ty) -> kind == Ty_name){
					if (type == init){
						EM_error(d->pos, "illegal type cycle");
						init -> u.name.ty = Ty_Int();
						break;
					}
				}
				types = types->tail;
			}
			break;
		}
		case A_functionDec: {
			A_fundecList funcs = d->u.function;

			// step 1: put function decs into the environment
			while(funcs){
				//if(S_look(venv, funcs->head->name) != NULL){
				//	EM_error(d->pos, "two functions have the same name");
				//	funcs = funcs->tail;
				//	continue;
				//}

				Ty_ty resultTy;
				if(funcs->head->result){
					resultTy = S_look(tenv, funcs->head->result);
					if(!resultTy){
						EM_error(funcs->head->pos, "undefined result type %s", S_name(funcs->head->result));
						resultTy = Ty_Void();
					}
				}
				else{
					resultTy = Ty_Void();
				}

				// create the new level
				Temp_label func_label = Temp_newlabel();
				U_boolList formal_escapes = makeEscapeList(funcs->head->params);
				Tr_level new_level = Tr_newLevel(level, func_label, formal_escapes);

				Ty_tyList formalTys = formalTyList(tenv, funcs->head->params);
				E_enventry entry = E_FunEntry(new_level, func_label, formalTys, resultTy);
				entry->kind = E_funEntry;
				S_enter(venv, funcs->head->name, entry);

				funcs = funcs->tail;
			}

			// step 2: check the function body validation
			funcs = d->u.function;
			while(funcs){
				E_enventry func_entry = S_look(venv, funcs->head->name);
				Tr_accessList formalAccessList = Tr_formals(func_entry->u.fun.level);

				A_fieldList formalNames = funcs->head->params;

				S_beginScope(venv);

				while(formalNames){
					Ty_ty type = S_look(tenv, formalNames->head->typ);
					E_enventry var_entry = E_VarEntry(formalAccessList->head, type);
					S_enter(venv, formalNames->head->name, var_entry);

					formalNames = formalNames->tail;
					formalAccessList = formalAccessList->tail;
				}

				struct expty exp = transExp(venv, tenv, funcs->head->body, func_entry->u.fun.level, nearest_break);
				E_enventry ent = S_look(venv, funcs->head->name);
				if(base_ty(ent->u.fun.result)->kind == Ty_void){
					// if return type is void
					if(base_ty(exp.ty)->kind != Ty_void){
						EM_error(funcs->head->pos, "procedure returns value");
					}
				}
				else{
					if(base_ty(ent->u.fun.result)->kind != base_ty(exp.ty)->kind){
						EM_error(funcs->head->pos, "procedure returns unexpected type");
					}
				}
				S_endScope(venv);

				//funcs = funcs->tail;

				// exit current program frag
				Tr_procEntryExit(func_entry->u.fun.level, exp.exp, formalAccessList);

				funcs = funcs->tail;
			}

			break; // break from the switch body
		}
	}
}
