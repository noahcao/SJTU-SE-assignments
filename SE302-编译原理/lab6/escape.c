#include <stdio.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "escape.h"
#include "table.h"

typedef struct escapeEntry_ {
    int depth;
    bool *esc;
} *escapeEntry;

static escapeEntry EscapeEntry(int d, bool *esc) {
    escapeEntry p = (escapeEntry)checked_malloc(sizeof(*p));
    p->depth = d;
    p->esc = esc;
    return p;
}

static void findEscape_inDec(S_table env, int depth, A_dec d);
static void findEscape_inVar(S_table env, int depth, A_var v);

static void findEscape_inExp(S_table env, int depth, A_exp exp){
    switch (exp->kind){
        case A_varExp: 
            findEscape_inVar(env, depth, exp->u.var);
            break;
        case A_letExp:{
            S_beginScope(env);
            A_decList decs = exp->u.let.decs;
            for(; decs; decs = decs->tail){
                findEscape_inDec(env, depth, decs->head);
            }
            findEscape_inExp(env, depth, exp->u.let.body);
            S_endScope(env);
            break;
        }
        case A_callExp:{
            A_expList args = exp->u.call.args;
            for(; args; args = args->tail){
                findEscape_inExp(env, depth, args->head);
            }
            break;
        }
        case A_opExp: {
            findEscape_inExp(env, depth, exp->u.op.left);
            findEscape_inExp(env, depth, exp->u.op.right);
            break;
        }
        case A_recordExp: {
            A_efieldList fields = exp->u.record.fields;
            for(; fields; fields = fields->tail){
                findEscape_inExp(env, depth, fields->head->exp);
            }
            break;
        }
        case A_seqExp: {
            A_expList exps = exp->u.seq;
            for(; exps; exps = exps->tail){
                findEscape_inExp(env, depth, exps->head);
            }
            break;
        }
        case A_assignExp:{
            findEscape_inExp(env, depth, exp->u.assign.exp);
            findEscape_inVar(env, depth, exp->u.assign.var);
            break;
        }
        case A_ifExp:{
            findEscape_inExp(env, depth, exp->u.iff.test);
            findEscape_inExp(env, depth, exp->u.iff.then);
            if(exp->u.iff.elsee){
                findEscape_inExp(env, depth, exp->u.iff.elsee);
            }
            break;
        }
        case A_whileExp:{
            findEscape_inExp(env, depth, exp->u.whilee.test);
            findEscape_inExp(env, depth, exp->u.whilee.body);
            break;
        }
        case A_forExp:{
            findEscape_inExp(env, depth, exp->u.forr.lo);
            findEscape_inExp(env, depth, exp->u.forr.hi);
            S_beginScope(env);
            S_enter(env, exp->u.forr.var, EscapeEntry(depth, &exp->u.forr.escape));
            S_endScope(env);
            break;
        }
        case A_arrayExp:{
            findEscape_inExp(env, depth, exp->u.array.size);
            findEscape_inExp(env, depth, exp->u.array.init);
            break;
        }
        default: break;
    }
}

static void findEscape_inDec(S_table env, int depth, A_dec d){
    switch(d->kind){
        case A_varDec: {
            findEscape_inExp(env, depth, d->u.var.init);
            S_enter(env, d->u.var.var, EscapeEntry(depth, &d->u.var.escape));
            break;
        }   
        case A_functionDec: {
            A_fundecList funs = d->u.function;
            for (; funs; funs = funs->tail) {
                S_beginScope(env);
                A_fieldList fields = funs->head->params;
                for (; fields; fields = fields->tail) {
                    A_field f = fields->head;
                    S_enter(env, f->name, EscapeEntry(depth + 1, &f->escape));
                }
                findEscape_inExp(env, depth + 1, funs->head->body);
                S_endScope(env);
            }
            break;
        }
        default: break;
    }
}

static void findEscape_inVar(S_table env, int depth, A_var v){
    switch(v->kind){
        case A_simpleVar:{
            escapeEntry entry = S_look(env, v->u.simple);
            if(!entry){
                printf("%s: not found in the S_table env\n", S_name(v->u.simple));
                assert(entry);
            }
            if(entry->depth!=depth){
                printf("%s: escape found\n", S_name(v->u.simple));
                *(entry->esc) = TRUE;
            }
            break;
        }
        case A_fieldVar: {
            findEscape_inVar(env, depth, v->u.field.var);
            break;
        }
        case A_subscriptVar: {
            findEscape_inVar(env, depth, v->u.subscript.var);
            findEscape_inExp(env, depth, v->u.subscript.exp);
            break;
        }
    }
}

void Esc_findEscape(A_exp exp) {
	findEscape_inExp(S_empty(), 0, exp);
}

