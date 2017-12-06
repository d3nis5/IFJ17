/**
 * Názov: symtable.c
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


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


SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, char type)
{
	if ( *RootPtr == NULL )
	{
		SYMTB_itemptr_l new_var = (SYMTB_itemptr_l) malloc(sizeof(struct local_symtable_item));
		if ( new_var == NULL )
		{
			err_code = COMPILER_ERR; 
			return NULL;
		}
		
		new_var->var_name = name;
		new_var->type = type;
		new_var->lptr = NULL;
		new_var->rptr = NULL;
		*RootPtr = new_var;
		return *RootPtr;
	}
	else
	{
		if ( strcmp(name, (*RootPtr)->var_name) < 0 )
		{
			LST_add_var(&((*RootPtr)->lptr), name, type);
		}
		else if ( strcmp(name, (*RootPtr)->var_name) > 0 )
		{
			LST_add_var(&((*RootPtr)->rptr), name, type);
		}
		else
		{
			err_code = -1;
			return (*RootPtr);
		}
	}
}

void LST_delete_tab(SYMTB_itemptr_l *RootPtr)
{
	while(*RootPtr != NULL)
	{
		SYMTB_itemptr_l lptr = (*RootPtr)->lptr;
		SYMTB_itemptr_l rptr = (*RootPtr)->rptr;

		LST_delete_tab(&lptr);
		LST_delete_tab(&rptr);
		/*if((*RootPtr)->var_name != NULL)
		{
			free((*RootPtr)->var_name);
			(*RootPtr)->var_name = NULL;
		}
		*/
		free(*RootPtr);
		*RootPtr = NULL;
	}

}


/* ----------FUNKCIE PRE PRACU S GLOBALNOU TABULKOU SYMBOLOV---------- */


void GST_init(SYMTB_itemptr_g *RootPtr)
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
				return GST_search(RootPtr->lptr, name);
			}
			else
			{
				return GST_search(RootPtr->rptr, name);
			}
		}
	}
	return NULL;
}

SYMTB_itemptr_g GST_add_function(SYMTB_itemptr_g *RootPtr, char *name, bool declared, bool defined, char type, char *params)
{
	if ( *RootPtr == NULL )
	{
		SYMTB_itemptr_g new_function = (SYMTB_itemptr_g) malloc(sizeof(struct global_symtable_item));
		if ( new_function == NULL )
		{
			err_code = COMPILER_ERR; 
			return NULL;
		}

		new_function->function_name = name;
		new_function->fc_declared = declared;
		new_function->fc_defined = defined;
		new_function->ret_type = type;
		new_function->par_count = 0;

		int i = 0;
		while(params[i] != 0)
		{
			if(GST_add_par(new_function, params[i]) == false)
			{
				err_code = COMPILER_ERR;
				return false;
			}
			i++;
		}

		new_function->local_symtb = NULL;
		new_function->lptr = NULL;
		new_function->rptr = NULL;
		*RootPtr = new_function;
		return *RootPtr;
	}
	else
	{
		if ( strcmp(name, (*RootPtr)->function_name) < 0 )
		{
			GST_add_function(&((*RootPtr)->lptr), name, declared, defined, type, params);
		}
		else if ( strcmp(name, (*RootPtr)->function_name) > 0 )
		{
			GST_add_function(&((*RootPtr)->rptr), name, declared, defined, type, params);
		}
		else
		{
			err_code = -1;
			return (*RootPtr);
		}
	}
}

bool GST_add_par(SYMTB_itemptr_g function, char type)
{
	if ( function->par_count >= (MAX_PAR-1) )
	{
		return false;
	}
	else
	{
		function->parameters[function->par_count] = type;
		function->par_count++;
		function->parameters[function->par_count] = 0;
	}
	return true;
}

bool GST_add_builtin(SYMTB_itemptr_g *table)
{
	SYMTB_itemptr_g function = NULL;

	/* Length(s As String) As Integer */

	builtin1 = (char*) malloc(7 * sizeof(char));
	strcpy(builtin1, "length");
	function = GST_add_function(table, builtin1, true, true, 'i', "s");

	function->par_names[0] = "s";

	/* SubStr(s As String, i as Integer, n As Integer) As String */

	builtin2 = (char*) malloc(7 * sizeof(char));
	strcpy(builtin2, "substr");
	function = GST_add_function(table, builtin2, true, true, 's', "sii");

	function->par_names[0] = "s";
	function->par_names[1] = "i";
	function->par_names[2] = "n";

	/* Asc(s As String, i As Integer) As Integer */
	 
	builtin3 = (char*) malloc(4 * sizeof(char));
	strcpy(builtin3, "asc");
	function = GST_add_function(table, builtin3, true, true, 'i', "si");

	function->par_names[0] = "s";
	function->par_names[1] = "i";

	/* Chr(i As Integer) As String */

	builtin4 = (char*) malloc(4 * sizeof(char));
	strcpy(builtin4, "chr");
	function = GST_add_function(table, builtin4, true, true, 's', "i");	

	function->par_names[0] = "i";
}

void GST_delete_tab(SYMTB_itemptr_g *RootPtr)
{
	while(*RootPtr != NULL)
	{
		SYMTB_itemptr_g lptr = (*RootPtr)->lptr;
		SYMTB_itemptr_g rptr = (*RootPtr)->rptr;

		GST_delete_tab(&lptr);
		GST_delete_tab(&rptr);
		if((*RootPtr)->local_symtb != NULL)
		{
			LST_delete_tab(&((*RootPtr)->local_symtb));
			(*RootPtr)->local_symtb = NULL; 
		}
		free(*RootPtr);
		*RootPtr = NULL;
	}

}

void GST_free_builtin_names()
{
	free(builtin1);
	free(builtin2);
	free(builtin3);
	free(builtin4);
}
