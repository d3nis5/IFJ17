#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>

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
        printf("%s  +-[%s]\n", sufix, TempTree->function_name);
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


SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, bool declared, bool defined, DT_type type)
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

SYMTB_itemptr_g GST_add_function(SYMTB_itemptr_g *RootPtr, char *name, bool declared, bool defined, DT_type type)
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
			GST_add_function(&((*RootPtr)->lptr), name, declared, defined, type);
		}
		else if ( strcmp(name, (*RootPtr)->function_name) > 0 )
		{
			GST_add_function(&((*RootPtr)->rptr), name, declared, defined, type);
		}
		else
		{
			return (*RootPtr);
		}
	}
}

void GST_delete_tab(SYMTB_itemptr_g *RootPtr)
{
	while(*RootPtr != NULL)
	{
		SYMTB_itemptr_g lptr = (*RootPtr)->lptr;
		SYMTB_itemptr_g rptr = (*RootPtr)->rptr;

		GST_delete_tab(&lptr);
		GST_delete_tab(&rptr);
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
	
	GST_add_function(&root, "hello", false, false, TYPE_integer);
	temp = GST_add_function(&root, "world", false, false, TYPE_integer);
	if (temp == NULL)
		printf("OK\n");
	else
	{
		printf("ERROR\n");
		printf("%s\n", temp->function_name);
	}
	GST_add_function(&root, "hell", false, false, TYPE_integer);
	GST_add_function(&root, "a", false, false, TYPE_integer);
	GST_add_function(&root, "b", false, false, TYPE_integer);
	GST_add_function(&root, "c", false, false, TYPE_integer);
	GST_add_function(&root, "var_d", false, false, TYPE_integer);
	GST_add_function(&root, "hell", false, false, TYPE_integer);
	temp = GST_add_function(&root, "world", false, false, TYPE_integer);
	if (temp == NULL)
		printf("OK\n");
	else
	{
		printf("ERROR\n");
		printf("%s\n", temp->function_name);
	}
	

	Print_tree_g(root);
	
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
}
*/
