#include "instructions.h"

void init_list(instruction_lst *list)
{
	list->first = NULL;
	list->last = NULL;
}


bool add_instruction(instruction_lst *list, char *inst)
{
	char *new_instruction = malloc(strlen(inst) * sizeof(char) + 1);
	if ( new_instruction == NULL )
	{
		err_code = COMPILER_ERR;
		return false;
	}
	strcpy(new_instruction, inst);
	instruction_t *new = malloc(sizeof(instruction_t));
	if ( new == NULL )
	{
		err_code = COMPILER_ERR;
		return false;
	}

	new->ins = new_instruction;
	new->next = NULL;

	if ( list->last != NULL )
		list->last->next = new;
	list->last = new;
	
	
	if ( list->first == NULL )	/* zoznam bol prazdny */
	{
		list->first = new;
	}

	return true;
}

bool generate_main(instruction_lst *list)
{
	if ( add_instruction(list, "label main") == false )
		return false;
	if ( add_instruction(list, "createframe") == false )
		return false;
	if ( add_instruction(list, "pushframe") == false )
		return false;
	if ( add_instruction(list, "defvar lf@$result") == false )
		return false;

	return true;
}

void print_list(instruction_lst list)
{
	instruction_t *tmp = list.first;
	
	while ( tmp != NULL )
	{
		printf("%s\n", tmp->ins);
		tmp = tmp->next;	
	}
}

void delete_list(instruction_lst *list)
{
	if ( list->first == NULL )	/* prazdny zoznam */
		return;
	instruction_t *act = list->first;	/* aktualny prvok */
	instruction_t *next = act->next;	/* nasledujuci prvok */

	while ( next != NULL )
	{
		free(act->ins);		
		free(act);
		act = next;
		next = act->next;
	}
	free(act->ins);		
	free(act);
	free(list);
	list = NULL;
}
