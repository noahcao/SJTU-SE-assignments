
/*Lab5: This header file is not complete. Please finish it with more definition.*/

#ifndef FRAME_H
#define FRAME_H

#include "tree.h"
#include "temp.h"
#include "util.h"
#include "assem.h"

// define registers in X86 
#define rbx (F_RBX())
#define r8 (F_R8())
#define r9 (F_R9())
#define r10 (F_R10())
#define r11 (F_R11())
#define r12 (F_R12())
#define r13 (F_R13())
#define r14 (F_R14())
#define r15 (F_R15())
#define rdi (F_RDI())
#define rsi (F_RSI())
#define rcx (F_RCX())
#define rdx (F_RDX())
#define rax (F_RAX())
#define rsp (F_RSP())
#define rbp (F_RBP())

Temp_map F_tempMap;
extern const int F_wordSize;

typedef struct F_frame_ *F_frame;

typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;

struct F_accessList_ {F_access head; F_accessList tail;};

struct F_access_
{
	enum
	{
		inFrame,
		inReg
	} kind;
	union {
		int offset;		 //inFrame
		Temp_temp reg; //inReg
	} u;
};

struct F_frame_
{
	Temp_label name;
	F_accessList formals;
	F_accessList locals;

	// the number of arguments
	int argSize;
	//the number of local variables
	int length;

	//register lists for the frame
	Temp_tempList calleesaves;
	Temp_tempList callersaves;
	Temp_tempList specialregs;
};

Temp_label F_name(F_frame f);

F_frame F_newFrame(Temp_label name, U_boolList formals);
F_accessList F_formals(F_frame f);
F_accessList F_AccessList(F_access head, F_accessList tail);
F_access F_allocLocal(F_frame f, bool escape);


/* declaration for fragments */
typedef struct F_frag_ *F_frag;
struct F_frag_ {enum {F_stringFrag, F_procFrag} kind;
			union {
				struct {Temp_label label; string str;} stringg;
				struct {T_stm body; F_frame frame;} proc;
			} u;
};

F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ 
{
	F_frag head; 
	F_fragList tail;
};

F_fragList F_FragList(F_frag head, F_fragList tail);


Temp_temp F_FP();
Temp_temp F_SP();
Temp_temp F_RV();

Temp_temp F_RAX();
Temp_temp F_R10();
Temp_temp F_R11();
Temp_temp F_R12();
Temp_temp F_R13();
Temp_temp F_R14();
Temp_temp F_R15();
Temp_temp F_RBX();
Temp_temp F_RBP();
Temp_temp F_RSP();
Temp_temp F_RDI();
Temp_temp F_RSI();
Temp_temp F_RDX();
Temp_temp F_RCX();
Temp_temp F_R8();
Temp_temp F_R9();

T_exp F_exp(F_access access, T_exp fp);
T_exp F_externalCall(string s, T_expList args);
AS_proc F_procEntryExit3(F_frame f, AS_instrList body);
#endif

