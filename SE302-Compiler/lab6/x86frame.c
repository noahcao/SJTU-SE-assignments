#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include "tree.h"
#include "frame.h"
#include "errormsg.h"

static int F_wordsize = 8;

Temp_label F_name(F_frame f){
	return f->name;
}

static F_access InFrame(int offset){
	F_access access = checked_malloc(sizeof(*access));
	access->kind = inFrame;
	access->u.offset = offset;
	return access;
}

static F_access InReg(Temp_temp reg){
	F_access access = checked_malloc(sizeof(*access));
	access->kind = inReg;
	access->u.reg = reg;
	return access;
}

static F_accessList makeFAccessList(F_frame f, U_boolList formals, int *argSize, int offset){
	if (formals == NULL) return NULL;

	(*argSize)++;

	if (formals->head){
		if (offset == F_wordsize || (*argSize) > 7){
			return F_AccessList(InFrame(offset), makeFAccessList(f, formals->tail, argSize, offset + F_wordsize));
		}
		return F_AccessList(F_allocLocal(f, TRUE), makeFAccessList(f, formals->tail, argSize, offset));
	}
	else{
		return F_AccessList(InReg(Temp_newtemp()), makeFAccessList(f, formals->tail, argSize, offset));
	}
}

F_frame F_newFrame(Temp_label name, U_boolList formals){
	F_frame frame = checked_malloc(sizeof(*frame));
	frame->name = name;

	frame->length = 0;

	int *argSize = checked_malloc(sizeof(*argSize));
	*argSize = 0;
	frame->formals = makeFAccessList(frame, formals, argSize, F_wordsize);
	frame->argSize = *argSize;

    frame->calleesaves = NULL;
	frame->callersaves = NULL;
	frame->specialregs = NULL;

	return frame;
}

F_accessList F_formals(F_frame f){
	return f->formals;
}

F_accessList F_AccessList(F_access head, F_accessList tail){
	F_accessList accessList = checked_malloc(sizeof(*accessList));
	accessList->head = head;
	accessList->tail = tail;
	return accessList;
}

F_access F_allocLocal(F_frame f, bool escape){
	F_access access;
	if (escape){
		f->length += F_wordsize;
		access = InFrame(-f->length);
	}
	else{
		access = InReg(Temp_newtemp());
	}
	return access;
}

F_frag F_StringFrag(Temp_label label, string str){
	F_frag frag = checked_malloc(sizeof(*frag));
	frag->kind = F_stringFrag;
	frag->u.stringg.label = label;
	frag->u.stringg.str = str;
	return frag;
}

F_frag F_ProcFrag(T_stm body, F_frame frame){
	F_frag frag = checked_malloc(sizeof(*frag));
	frag->kind = F_procFrag;
	frag->u.proc.body = body;
	frag->u.proc.frame = frame;
	return frag;
}

F_fragList F_FragList(F_frag head, F_fragList tail){
	F_fragList fragList = checked_malloc(sizeof(*fragList));
	fragList->head = head;
	fragList->tail = tail;
	return fragList;
}

// ---------- supported registers in the X86 architcture -----
Temp_temp F_FP(){ return F_RBP();}
Temp_temp F_SP(){ return F_RSP();}
Temp_temp F_RV(){ return F_RAX();}

Temp_temp F_RAX(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rax"));
	}
	return reg;
}

Temp_temp F_R8(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r8"));
	}
	return reg;
}

Temp_temp F_R9(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r9"));
	}

	return reg;
}

Temp_temp F_R10(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r10"));
	}
	return reg;
}

Temp_temp F_R11(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r11"));
	}
	return reg;
}

Temp_temp F_R12(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r12"));
	}
	return reg;
}

Temp_temp F_R13(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r13"));
	}
	return reg;
}

Temp_temp F_R14(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r14"));
	}
	return reg;
}

Temp_temp F_R15(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%r15"));
	}

	return reg;
}

Temp_temp F_RBX(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rbx"));
	}
	return reg;
}

Temp_temp F_RBP(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rbp"));
	}

	return reg;
}

Temp_temp F_RSP(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rsp"));
	}
	return reg;
}

Temp_temp F_RDI(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rdi"));
	}
	return reg;
}

Temp_temp F_RSI(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rsi"));
	}
	return reg;
}

Temp_temp F_RDX(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rdx"));
	}
	return reg;
}

Temp_temp F_RCX(){
	static Temp_temp reg = NULL;
	if (reg == NULL){
		reg = Temp_newtemp();
		Temp_enter(F_tempMap, reg, String("%rcx"));
	}
	return reg;
}

T_exp F_exp(F_access access, T_exp fp){
	switch (access->kind){
        case inReg: return T_Temp(access->u.reg);
        case inFrame: return T_Mem(T_Binop(T_plus, T_Const(access->u.offset), fp));
        default:{
            printf("frame: unknown access kind\n");
            assert(0);
        }
	}
}

T_exp F_externalCall(string s, T_expList args){
	return T_Call(T_Name(Temp_namedlabel(s)), T_ExpList(T_Const(0), args));
}

AS_proc F_procEntryExit3(F_frame f, AS_instrList body){
	string p, e;
	char inst1[256], inst2[256];

	sprintf(inst1, "subq $%d, %rsp\n", f->length);
	sprintf(inst2, "");

	p = String(inst1);
	e = String(inst2);

	return AS_Proc(p, body, e);
}
