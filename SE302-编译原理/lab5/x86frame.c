#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include "tree.h"
#include "frame.h"

/*Lab5: Your implementation here.*/
static int F_wordsize = 8;

//varibales
struct F_access_ {
	enum {inFrame, inReg} kind;
	union {
		int offset; //inFrame
		Temp_temp reg; //inReg
	} u;
};

struct F_frame_{
	Temp_label name;
	F_accessList formals;
	F_accessList locals;

	int argNumber; // number of arguments 
	int localSize; // size of local variables

	Temp_tempList callee_saved;
	Temp_tempList caller_saved;
	Temp_tempList regs;
};

Temp_label F_name(F_frame frame){
	return frame->name;
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

F_accessList F_AccessList(F_access head, F_accessList tail){
	F_accessList access_List = checked_malloc(sizeof(*access_List));
	access_List->head = head;
	access_List->tail = tail;
	return access_List;
}

static F_accessList buildAccessList(U_boolList formals, int *argNumber, int offset){
	if(formals == NULL) return NULL;

	*argNumber += 1;

	if(formals->head){
		return F_AccessList(InFrame(offset), buildAccessList(formals->tail, argNumber, offset + F_wordsize));
	}
	else{
		return F_AccessList(InReg(Temp_newtemp()), buildAccessList(formals->tail, argNumber, offset));
	}
}

F_frame F_newFrame(Temp_label name, U_boolList formals){
	F_frame frame = checked_malloc(sizeof(*frame));
	frame->name = name;

	int *argnum = checked_malloc(sizeof(*argnum));
	*argnum = 0;
	frame->formals = buildAccessList(formals, argnum, F_wordsize);
	frame->argNumber = *argnum;
	frame->localSize = 0;

	frame->callee_saved = NULL;
	frame->caller_saved = NULL;
	frame->regs = NULL;

	return frame;
}

F_accessList F_formals(F_frame frame){
	return frame->formals;
}

F_access F_allocLocal(F_frame frame, bool escape){
	F_access access;
	if(escape){
		access = InFrame(frame->localSize);
		frame->localSize += F_wordsize;
	}
	else{
		access = InReg(Temp_newtemp());
	}
	return access;
}


F_frag F_StringFrag(Temp_label label, string str) {
	F_frag frag = checked_malloc(sizeof(*frag));
	frag->kind = F_stringFrag;
	frag->u.stringg.label = label;
	frag->u.stringg.str = str;   
	return frag;                                   
}                                                     
                                                      
F_frag F_ProcFrag(T_stm body, F_frame frame) {        
	F_frag frag = checked_malloc(sizeof(*frag));
	frag->kind = F_procFrag;
	frag->u.proc.body = body;
	frag->u.proc.frame = frame;
	return frag;                                     
}                                                     
                                                      
F_fragList F_FragList(F_frag head, F_fragList tail) { 
	F_fragList fragList = checked_malloc(sizeof(*fragList));
	fragList->head = head;
	fragList->tail = tail;
	return fragList;                               
}                                                     

Temp_temp F_FP(){return Temp_newtemp();}
Temp_temp F_SP(){return Temp_newtemp();}
Temp_temp F_RV(){return Temp_newtemp();}
Temp_temp F_PC(){return Temp_newtemp();}

T_exp F_exp(F_access access, T_exp fp){
	switch(access->kind){
		case inReg: return T_Temp(access->u.reg);
		case inFrame: return T_Mem(T_Binop(T_plus, T_Const(access->u.offset), fp));
		default:{
			printf("access' kind shoud be frame/registor\n");
		}
	}
}

T_exp F_externalCall(string s, T_expList args){
	return T_Call(T_Name(Temp_namedlabel(s)), args);
}