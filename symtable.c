#include "symtable.h"


/* TODO zmazat */
void Print_tree2_l(SYMTB_itemptr_l TempTree, char* sufix, char fromdir)
/* vykresli sktrukturu binarniho stromu */

{
     if (TempTree != NULL)
     {
	char* suf2 = (char*) malloc(strlen(sufix) + 4);
	strcpy(suf2, sufix);
        if (fromdir == 'L')
	{
	   suf2 = strcat(suf2, "  |");
	   printf("%s\n", suf2);
	}
	else
	   suf2 = strcat(suf2, "   ");
	Print_tree2_l(TempTree->rptr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->var_name);
	strcpy(suf2, sufix);
        if (fromdir == 'R')
	   suf2 = strcat(suf2, "  |");	
	else
	   suf2 = strcat(suf2, "   ");
	Print_tree2_l(TempTree->lptr, suf2, 'L');
	if (fromdir == 'R') printf("%s\n", suf2);
	free(suf2);
    }
}
/* TODO zmazat */
void Print_tree_l(SYMTB_itemptr_l TempTree)
{
  printf("Struktura binarniho stromu:\n");
  printf("\n");
  if (TempTree != NULL)
     Print_tree2_l(TempTree, "", 'X');
  else
     printf("strom je prazdny\n");
  printf("\n");
  printf("=================================================\n");
} 


/* TODO zmazat */
void Print_tree2_g(SYMTB_itemptr_g TempTree, char* sufix, char fromdir)
/* vykresli sktrukturu binarniho stromu */

{
     if (TempTree != NULL)
     {
	char* suf2 = (char*) malloc(strlen(sufix) + 4);
	strcpy(suf2, sufix);
        if (fromdir == 'L')
	{
	   suf2 = strcat(suf2, "  |");
	   printf("%s\n", suf2);
	}
	else
	   suf2 = strcat(suf2, "   ");
	Print_tree2_g(TempTree->rptr, suf2, 'R');
        printf("%s  +-[%s(%s)]\n", sufix, TempTree->function_name,TempTree->parameters);
	strcpy(suf2, sufix);
        if (fromdir == 'R')
	   suf2 = strcat(suf2, "  |");	
	else
	   suf2 = strcat(suf2, "   ");
	Print_tree2_g(TempTree->lptr, suf2, 'L');
	if (fromdir == 'R') printf("%s\n", suf2);
	free(suf2);
    }
}
/* TODO zmazat */
void Print_tree_g(SYMTB_itemptr_g TempTree)
{
  printf("Struktura binarniho stromu:\n");
  printf("\n");
  if (TempTree != NULL)
     Print_tree2_g(TempTree, "", 'X');
  else
     printf("strom je prazdny\n");
  printf("\n");
  printf("=================================================\n");
} 







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


SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, bool declared, bool defined, char type)
{
	if ( *RootPtr == NULL )
	{
		SYMTB_itemptr_l new_var = (SYMTB_itemptr_l) malloc(sizeof(struct local_symtable_item));
		if ( new_var == NULL )
		{
			//err_code = COMPILER_ERR; 
			return NULL;
		}

		new_var->var_name = name;
		new_var->var_declared = declared;
		new_var->var_defined = defined;
		new_var->type = type;
		new_var->lptr = NULL;
		new_var->rptr = NULL;
		*RootPtr = new_var;
		return NULL;
	}
	else
	{
		if ( strcmp(name, (*RootPtr)->var_name) < 0 )
		{
			LST_add_var(&((*RootPtr)->lptr), name, declared, defined, type);
		}
		else if ( strcmp(name, (*RootPtr)->var_name) > 0 )
		{
			LST_add_var(&((*RootPtr)->rptr), name, declared, defined, type);
		}
		else
		{
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
		if((*RootPtr)->var_name != NULL)
		{
			free((*RootPtr)->var_name);
			(*RootPtr)->var_name = NULL;
		}

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
			//err_code = COMPILER_ERR; 
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
			GST_add_par(new_function, params[i]);
			i++;
		}

		new_function->local_symtb = NULL;
		new_function->lptr = NULL;
		new_function->rptr = NULL;
		*RootPtr = new_function;
		return NULL;
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
			return (*RootPtr);
		}
	}
}

bool GST_add_par(SYMTB_itemptr_g function, char type)
{
	if ( function->par_count >= MAX_PAR )
	{
		return false;
	}
	else
	{
		function->parameters[function->par_count] = type;
		function->par_count++;

	function->parameters[function->par_count] = 0;	/* TODO zmazat, len pre vypis */
	}
	return true;
}

bool GST_add_builtin(SYMTB_itemptr_g *table)
{
	/* Length(s As String) As Integer */

	char *name = (char*) malloc(7 * sizeof(char));
	strcpy(name, "length");
	GST_add_function(table, name, true, true, 'i', "s");

	/* SubStr(s As String, i as Integer, n As Integer) As String */

	name = (char*) malloc(7 * sizeof(char));
	strcpy(name, "substr");
	GST_add_function(table, name, true, true, 's', "sii");

	/* Asc(s As String, i As Integer) As Integer */
	 
	name = (char*) malloc(4 * sizeof(char));
	strcpy(name, "asc");
	GST_add_function(table, name, true, true, 'i', "si");

	/* Chr(i As Integer) As String */

	name = (char*) malloc(4 * sizeof(char));
	strcpy(name, "chr");
	GST_add_function(table, name, true, true, 's', "i");	
}

void GST_delete_tab(SYMTB_itemptr_g *RootPtr)
{
	while(*RootPtr != NULL)
	{
		SYMTB_itemptr_g lptr = (*RootPtr)->lptr;
		SYMTB_itemptr_g rptr = (*RootPtr)->rptr;

		GST_delete_tab(&lptr);
		GST_delete_tab(&rptr);
		if((*RootPtr)->function_name != NULL)
		{
			free((*RootPtr)->function_name);
			(*RootPtr)->function_name = NULL;
		}
		if((*RootPtr)->local_symtb != NULL)
		{
			LST_delete_tab(&((*RootPtr)->local_symtb));
			(*RootPtr)->local_symtb = NULL; 
		}
		free(*RootPtr);
		*RootPtr = NULL;
	}

}

/* -------------------------------TEST---------------------------------- 
int main()
{	
	SYMTB_itemptr_g root;
	SYMTB_itemptr_g temp;
	GST_init(&root);
	
	GST_add_builtin(&root);
	
	GST_add_function(&root, "hello", false, false, 'i', "isdi");
	GST_add_par(root, 'i');
	GST_add_par(root, 's');
	GST_add_par(root, 'd');
	GST_add_par(root, 'i');
	temp = GST_add_function(&root, "world", false, false, 'i');
	if (temp == NULL)
		printf("OK\n");
	else
	{
		printf("ERROR\n");
		printf("%s\n", temp->function_name);
	}
	GST_add_function(&root, "hell", false, false, 'i', "");
	GST_add_function(&root, "a", false, false, 'i');
	temp = GST_search(root, "a");
	GST_add_par(temp, 'd');
	GST_add_par(temp, 'i');
	GST_add_function(&root, "b", false, false, 'i');


	GST_add_function(&root, "c", false, false, 'i');
	GST_add_function(&root, "var_d", false, false, 'i');
	GST_add_function(&root, "hell", false, false, 'i');
	temp = GST_add_function(&root, "world", false, false, 'i');
	if (temp == NULL)
		printf("OK\n");
	else
	{
		printf("ERROR\n");
		printf("%s\n", temp->function_name);
	}
	
	Print_tree_g(root);
	GST_delete_tab(&root);

	temp = GST_search(root, "a");
	if(temp == NULL)
		printf("not found\n");
	else
		printf("found : %s\n", temp->function_name);

	temp = GST_search(root, "b");
	if(temp == NULL)
		printf("not found\n");
	else
		printf("found : %s\n", temp->function_name);
	GST_delete_tab(&root);

	temp = GST_search(root, "world");
	if(temp == NULL)
		printf("not found\n");
	else
		printf("found : %s\n", temp->function_name);
	Print_tree_g(root);

	return 0;
}*/

