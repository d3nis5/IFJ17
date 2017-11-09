#include "symtable.h"


/* ----------FUNKCIE PRE PRACU S LOKALNOU TABULKOU SYMBOLOV---------- */


void LST_init(SYMTB_itemptr_l *RootPtr)
{
	*RootPtr = NULL;
}

SYMTB_itemptr_l LST_search(SYMTB_itemptr_l RootPtr, char *name)
{
	
	if ( RootPtr == NULL )
		return NULL;
	else
	{
		if ( strcmp(name, RootPtr->var_name) == 0 )
		{
			return RootPtr;
		}
		else
		{
			if ( strcmp(name, RootPtr->var_name) < 0)
			{
				return LST_search(RootPtr->lptr, name);
			}
			else
			{
				return LST_search(RootPtr->rptr, name);
			}
		}
	}
}


SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, bool declared, bool defined, DT_type type)
{
	if ( *RootPtr == NULL )
	{
		SYMTB_itemptr_l new_var = (SYMTB_itemptr_l) malloc(sizeof(struct local_symtable_item));
		if ( new_var == NULL )
		{
			err_code = 
			return NULL;
		}
	}

}


/* ----------FUNKCIE PRE PRACU S GLOBALNOU TABULKOU SYMBOLOV---------- */


void GST_init(SYMTB_itemptr_g *)
{
	*RootPtr = NULL;
}

SYMTB_itemptr_g GST_search(SYMTB_itemptr_g RootPtr, char *name)
{
	if ( RootPtr == NULL )
		return NULL;
	else
	{
		if ( strcmp(name, RootPtr->function_name) == 0 )
		{
			return RootPtr;
		}
		else
		{
			if ( strcmp(name, RootPtr->function_name) < 0)
			{
				return LST_search(RootPtr->lptr, name);
			}
			else
			{
				return LST_search(RootPtr->rptr, name);
			}
		}
	}
}
