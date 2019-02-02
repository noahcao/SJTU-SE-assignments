#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "env.h"

/*Lab4: Your implementation of lab4*/

E_enventry E_VarEntry(Ty_ty ty)
{	
	E_enventry entry = checked_malloc(sizeof(*entry));
	entry -> u.var.ty = ty;

	return entry;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result)
{	
	E_enventry entry = checked_malloc(sizeof(*entry));
	entry -> kind = E_funEntry;
	entry -> u.fun.formals = formals;
	entry -> u.fun.result = result;

	return entry;
}

S_table E_base_tenv(void)
{	
	S_table table  = S_empty();
	S_symbol ty_int = S_Symbol("int");
	S_symbol ty_string = S_Symbol("string");

	S_enter(table, ty_int, Ty_Int());
	S_enter(table, ty_string, Ty_String());

	return table;
}

E_enventry E_ROVarEntry(Ty_ty ty)
{
	E_enventry entry = checked_malloc(sizeof(*entry));
	entry->u.var.ty = ty;
	entry->readonly = 1;
	return entry;
}
S_table E_base_venv(void)
{	
	S_table table = S_empty();
	S_symbol funname = S_Symbol("getcgar");
	Ty_tyList paras = NULL;
	Ty_ty result = Ty_String();

	S_enter(table, funname, E_FunEntry(paras, result));

	return table;	
}
