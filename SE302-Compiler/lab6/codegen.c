#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "errormsg.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "codegen.h"
#include "table.h"

#define TL(x,y) (Temp_TempList(x,y))

/*
    Use of registers:
    
*/

static void emit(AS_instr inst);
static void munchStm(T_stm s);
static Temp_temp munchExp(T_exp e);

static AS_instrList iList = NULL, last = NULL;
static AS_instrList used_args = NULL;

static void emit(AS_instr inst){
    if(last != NULL){
        last = last->tail = AS_InstrList(inst, NULL);
    }
    else{
        last = iList = AS_InstrList(inst, NULL);
    }
}

// imm->reg
AS_instr Imm2Reg(int imm, Temp_temp dst){
  char inst[256];
  sprintf(inst, "movq $%d, `d0", imm);
  return AS_Oper(String(inst), TL(dst, NULL), NULL, NULL);
}

// mem(r)->reg
AS_instr Mem2Reg(Temp_temp r, Temp_temp dst){
  char inst[256];
  sprintf(inst, "movq (`s0), `d0");
  return AS_Oper(String(inst), TL(dst, NULL), TL(r, NULL), NULL);
}

// mem(r + i)->reg
AS_instr MemImm2Reg(Temp_temp r, int imm, Temp_temp dst){
  char inst[256];
  sprintf(inst, "movq %d(`s0), `d0", imm);
  return AS_Oper(String(inst), TL(dst, NULL), TL(r, NULL), NULL);
}

// reg->reg
AS_instr Reg2Reg(Temp_temp src, Temp_temp dst){
  char inst[256];
  sprintf(inst, "movq `s0, `d0");
  return AS_Move(String(inst), TL(dst, NULL), TL(src, NULL));
}

AS_instr opImm(string op, Temp_temp dst, int imm){
  char inst[256];
  sprintf(inst, "%s $%d, `d0", op, imm);
  return AS_Oper(String(inst), TL(dst, NULL), TL(dst, NULL), NULL);
}

AS_instr opReg(string op, Temp_temp src, Temp_temp dst){
  char inst[256];
  sprintf(inst, "%s `s0, `d0", op);
  return AS_Oper(String(inst), TL(dst, NULL), TL(src, TL(dst, NULL)), NULL);
}

Temp_temp emitOpExp(string op, T_exp left_exp, T_exp right_exp){
    Temp_temp result_temp = Temp_newtemp();
    if (left_exp->kind != T_CONST && right_exp->kind != T_CONST){
        Temp_temp left_temp = munchExp(left_exp);
        Temp_temp right_temp = munchExp(right_exp);

        emit(Reg2Reg(left_temp, result_temp));
        emit(opReg(op, right_temp, result_temp));
        return result_temp;
    }

    Temp_temp another_temp;
    int imm;
    if (left_exp->kind == T_CONST){
        another_temp = munchExp(right_exp);
        imm = left_exp->u.CONST;
    }
    else{
        another_temp = munchExp(left_exp);
        imm = right_exp->u.CONST;
    }

    emit(Reg2Reg(another_temp, result_temp));
    emit(opImm(op, result_temp, imm));

    return result_temp;
}

// reverse pushq sequence
void pushArgs(AS_instrList instrs)
{
    if (instrs == NULL) return;
    pushArgs(instrs->tail);
    emit(instrs->head);
}

static Temp_tempList munchArgs(int index, T_expList args)
{
    if (args == NULL) return NULL;

    bool isConst = (args->head->kind == T_CONST);
    Temp_temp dst;
    switch (index){
        // first 6 args (except for static link) are stored in regs
        case 1: dst = rdi; break;
        case 2: dst = rsi; break;
        case 3: dst = rdx; break;
        case 4: dst = rcx; break;
        case 5: dst = r8; break;
        case 6: dst = r9; break;
        default:{
            dst = munchExp(args->head);
            AS_instr inst = AS_Oper("pushq `s0", TL(rsp, NULL), TL(dst, TL(rsp, NULL)), NULL);
            used_args = AS_InstrList(inst, used_args);
            return munchArgs(index + 1, args->tail);
        }
    }

    Temp_temp src = munchExp(args->head);
    emit(Reg2Reg(src, dst));

    return TL(dst, munchArgs(index + 1, args->tail));
}

static void munchMoveStm(T_stm s){
    T_exp dst_exp = s->u.MOVE.dst;
    T_exp src_exp = s->u.MOVE.src;
    char buf[256];

    if (dst_exp->kind == T_MEM){
        Temp_temp r = munchExp(dst_exp->u.MEM);
        if (src_exp->kind == T_CONST){
            //  imm -> mem(r)
            sprintf(buf, "movq $%d, (`s0)", src_exp->u.CONST);
            emit(AS_Oper(String(buf), NULL, TL(r, NULL), NULL));
            return;
        }
        else{
            // reg -> mem(r)
            sprintf(buf, "movq `s0, (`s1)");
            Temp_temp src = munchExp(src_exp);
            emit(AS_Oper(String(buf), NULL, TL(src, TL(r, NULL)), NULL));
            return;
        }
    }
    else{
        Temp_temp dst = munchExp(dst_exp);
        switch(src_exp->kind){
            case T_CONST: emit(Imm2Reg(src_exp->u.CONST, dst)); return; // imm->reg
            case T_MEM: emit(Mem2Reg(munchExp(src_exp->u.MEM), dst)); return; // mem->reg
            default: emit(Reg2Reg(munchExp(src_exp), dst)); return; // reg->reg
        }
    }
}

static void munchStm(T_stm s){
    char buf[256];
    switch (s->kind){
        case T_SEQ: assert(0);  // SEQ should have been eliminated
        case T_LABEL:{
            Temp_label label = s->u.LABEL;
            emit(AS_Label(Temp_labelstring(label), label));
            return;
        }
        case T_JUMP:{
            emit(AS_Oper("jmp `j0", NULL, NULL, AS_Targets(s->u.JUMP.jumps)));
            return;
        }
        case T_CJUMP:{
            Temp_temp left = munchExp(s->u.CJUMP.left);
            Temp_temp right = munchExp(s->u.CJUMP.right);

            emit(AS_Oper("cmpq `s0, `s1", NULL, TL(right, TL(left, NULL)), NULL));
            char *op;
            switch(s->u.CJUMP.op){
                case T_eq: op = "je"; break;
                case T_ne: op = "jne"; break;
                case T_lt: op = "jl"; break;
                case T_le: op = "jle"; break;
                case T_gt: op = "jg"; break;
                case T_ge: op = "jge"; break;
                case T_ult: op = "jb"; break;
                case T_ule: op = "jbe"; break;
                case T_ugt: op = "ja"; break;
                case T_uge: op = "jae"; break;
                default: assert(0);
            }
            // case 1: conditional true
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%s `j0", op);
            emit(AS_Oper(String(buf), NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));

            // case 2: conditional false 
            emit(AS_Oper(String("jmp `j0"), NULL, NULL, AS_Targets(Temp_LabelList(s->u.CJUMP.false, NULL))));
            return;
        }
        case T_MOVE:{ munchMoveStm(s); return;}
        case T_EXP: { munchExp(s->u.EXP); return;}
        default: assert(0);
    }
}

static Temp_temp munchOpExp(T_exp e){
    T_exp left_exp = e->u.BINOP.left;
    T_exp right_exp = e->u.BINOP.right;
    Temp_temp r = Temp_newtemp();
    switch (e->u.BINOP.op){
        case T_plus: return emitOpExp("addq", left_exp, right_exp);
        case T_minus: return emitOpExp("subq", left_exp, right_exp);
        case T_mul: return emitOpExp("imulq", left_exp, right_exp);
        case T_and: return emitOpExp("andq", left_exp, right_exp);
        case T_or: return emitOpExp("orq", left_exp, right_exp);
        case T_lshift: return emitOpExp("shlq", left_exp, right_exp);
        case T_rshift: return emitOpExp("shrq", left_exp, right_exp);
        case T_arshift: return emitOpExp("sarq", left_exp, right_exp);
        case T_xor: return emitOpExp("xorq", left_exp, right_exp);
        case T_div:{
            // division should be handled in special style
            Temp_temp left_temp = munchExp(left_exp);
            Temp_temp right_temp = munchExp(right_exp);

            emit(Reg2Reg(left_temp, rax));
            emit(AS_Oper("cqto", TL(rdx, NULL), NULL, NULL));
            emit(AS_Oper("idivq `s0", TL(rax, NULL), TL(right_temp, TL(rax, TL(rdx, NULL))), NULL));
            emit(Reg2Reg(rax, r));

            return r;
        }
        default: assert(0); // there is no other legal case
    }
}

static Temp_temp munchMemExp(T_exp e){
    Temp_temp dst = Temp_newtemp();
    Temp_temp r;
    T_exp mem = e->u.MEM;

    if (mem->kind == T_BINOP && mem->u.BINOP.op == T_plus){
        T_exp left = mem->u.BINOP.left;
        T_exp right = mem->u.BINOP.right;

        if (left->kind == T_CONST){
            r = munchExp(right);
            int imm = left->u.CONST;
            emit(MemImm2Reg(r, imm, dst));
            return dst;
        }

        if (right->kind == T_CONST){
            r = munchExp(left);
            int imm = right->u.CONST;
            emit(MemImm2Reg(r, imm, dst));
            return dst;
        }
    }
    r = munchExp(mem);
    emit(Mem2Reg(r, dst));

    return dst;
}

static Temp_temp munchCallExp(T_exp e){
    assert(e->u.CALL.fun->kind == T_NAME);

    // save caller-save registerss
    Temp_temp r11_temp = Temp_newtemp();
    Temp_temp r10_temp = Temp_newtemp();
    emit(Reg2Reg(r10, r10_temp));
    emit(Reg2Reg(r11, r11_temp));

    int arg_count = 0;
    used_args = NULL;
    Temp_temp dst = Temp_newtemp();
    Temp_tempList arg_temps = munchArgs(0, e->u.CALL.args);
    pushArgs(used_args);
    while (arg_temps){
        arg_count += 1;
        arg_temps = arg_temps->tail;
    }
    int arg_in_fram = arg_count > 7 ? arg_count - 6 : 1;

    // version #1:
    //Temp_tempList dst_regs = TL(rdi, TL(rsi, TL(rdx, TL(rax, TL(rcx, TL(r8, TL(r9, TL(r10, TL(r11, NULL)))))))));
    //Temp_tempList src_regs = munchArgs(0, e->u.CALL.args);

    // version #2:
    Temp_tempList dst_regs = TL(r10, TL(r11, TL(rax, NULL)));
    Temp_tempList src_regs = TL(rdi, TL(rsi, TL(rdx, TL(rcx, TL(r8, TL(r9, NULL))))));
    
    //Temp_tempList src_regs = NULL;
    AS_instr call = AS_Oper("call `j0", dst_regs, src_regs,
                            AS_Targets(Temp_LabelList(e->u.CALL.fun->u.NAME, NULL)));
    AS_instr ret = Reg2Reg(rax, dst);
    AS_instr expand_frame = opImm("addq", rsp, arg_in_fram * 8);

    emit(call);
    emit(ret);
    emit(expand_frame);

    // restore caller-save registers
    emit(Reg2Reg(r11_temp, r11));
    emit(Reg2Reg(r10_temp, r10));
    
    return dst;
}

static Temp_temp munchExp(T_exp e){
    char buf[256];
    switch (e->kind){
        case T_ESEQ: assert(0); // SEQ should have been all eliminated here
        case T_BINOP: return munchOpExp(e);
        case T_MEM: return munchMemExp(e);
        case T_TEMP: return e->u.TEMP;
        case T_CALL: return munchCallExp(e);
        case T_CONST:
        {
            Temp_temp dst = Temp_newtemp();
            emit(Imm2Reg(e->u.CONST, dst));
            return dst;
        }
        case T_NAME:
        {   
            Temp_temp dst = Temp_newtemp();
            string src = Temp_labelstring(e->u.NAME);   
            sprintf(buf, "leaq %s, `d0", src);
            emit(AS_Oper(String(buf), TL(dst, NULL), NULL, NULL));
            return dst;
        }
        default: assert(0);  // no other legal cases
    }
}

Temp_tempList calleeSaves(){
    // return all callee-save regs
    Temp_tempList callee_saves = TL(r12, TL(r13, TL(r14, TL(r15, TL(rbx, NULL)))));
    return callee_saves;
}

static Temp_tempList saveCalleesaves(){
    // save callee-regs in backup temps, and return the list of backups
    Temp_temp r12_bk = Temp_newtemp();
    Temp_temp r13_bk = Temp_newtemp();
    Temp_temp r14_bk = Temp_newtemp();
    Temp_temp r15_bk = Temp_newtemp();
    Temp_temp rbx_bk = Temp_newtemp();
    emit(Reg2Reg(r12, r12_bk));
    emit(Reg2Reg(r13, r13_bk));
    emit(Reg2Reg(r14, r14_bk));
    emit(Reg2Reg(r15, r15_bk));
    emit(Reg2Reg(rbx, rbx_bk));
    return TL(r12_bk, TL(r13_bk, TL(r14_bk, TL(r15_bk, TL(rbx_bk, NULL)))));
}

static void restoreCalleesaves(Temp_tempList temp_regs){
    // restore backup value into 5 callee-save regs
    Temp_tempList callee_saves = calleeSaves();
    while(temp_regs){
        assert(temp_regs != NULL);
        Temp_temp reg_bk = temp_regs->head;
        Temp_temp callee_save_reg = callee_saves->head;
        emit(Reg2Reg(reg_bk, callee_save_reg));
        temp_regs = temp_regs->tail;
        callee_saves = callee_saves->tail;
    }
}

static void arrangeFormals(F_accessList formals){
    int formal_num = 0;
    while(formals){
        F_access formal = formals->head;
        Temp_temp target_temp;
        switch(formal_num){
            case 1: target_temp = rdi; break;
            case 2: target_temp = rsi; break;
            case 3: target_temp = rdx; break;
            case 4: target_temp = rcx; break;
            case 5: target_temp = r8; break;
            case 6: target_temp = r9; break;
            default: {
                formal_num += 1;
                formals = formals->tail;
                continue;
            }
        }
        if (formal->kind == inReg){
            emit(Reg2Reg(target_temp, formal->u.reg));
        }
        else{
            T_stm s = T_Move(T_Mem(T_Binop(T_plus, T_Const(formal->u.offset), T_Temp(F_FP()))), T_Temp(target_temp));
            munchStm(s);
        }
        formal_num++;
        formals = formals->tail;
    }
}

AS_instrList F_codegen(F_frame f, T_stmList stmList)
{
    // initial instrList
    iList = NULL; last = NULL;
    F_accessList formals = F_formals(f);

    // step 1: organize input argments (formals)
    arrangeFormals(formals);

    // step 2: save callee-save registers
    Temp_tempList callee_baks = saveCalleesaves();

    // step 3: generate function body
    while (stmList){
        munchStm(stmList->head);
        stmList = stmList->tail;
    }

    // step 4: restore callee-save registers
    restoreCalleesaves(callee_baks);

    // step 5: handle return
    emit(Reg2Reg(rbp, rsp));
    emit(AS_Oper("ret", NULL, TL(F_SP(), TL(r12, TL(r13, TL(r14, TL(r15, TL(rbx, NULL)))))), NULL));
    return iList;
}
