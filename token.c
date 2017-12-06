/**
 * Názov: token.c
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */

#include "token.h"

void init_tkn_list(Ttoken_list *list)
{
	list->first = NULL;
	list->last = NULL;
}

bool add_token(Ttoken_list *list, Ttoken *token)
{
	/*if ( token->type == TKN_int )
		printf("%d %d\n", token->type, token->attribute.integer);
	else if ( token->type == TKN_dbl )
		printf("%d %g\n", token->type, token->attribute.dble);
	else
	{
		if ( token->attribute.string != NULL )
			printf("%d %s\n", token->type, token->attribute.string);
		else
			printf("token = %d\n", token->type);
	}*/

	Ttkn_list_item *new = malloc(sizeof(Ttkn_list_item));
	if ( new == NULL )
	{
		err_code = COMPILER_ERR;
		return false;
	}

	new->token = token;
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


void delete_tkn_list(Ttoken_list *list)
{
	if ( list->first == NULL )	/* prazdny zoznam */
	{
		return;
	}
	Ttkn_list_item *act = list->first;	/* aktualny prvok */
	Ttkn_list_item *next = act->next;	/* nasledujuci prvok */

	while ( next != NULL )
	{
		if ( ((act->token->type == TKN_str) || (act->token->type == TKN_id) ||
		(act->token->type == KWD_length) || (act->token->type == KWD_substr) ||
		(act->token->type == KWD_asc) || (act->token->type == KWD_chr)) && 
		(act->token->attribute.string != NULL))
		{
			free(act->token->attribute.string);
			act->token->attribute.string = NULL;
		}
		free(act->token);		
		free(act); 
		act = next;
		next = act->next;
	}
	if ( ((act->token->type == TKN_str) || (act->token->type == TKN_id)) && (act->token->attribute.string != NULL))
	{
		free(act->token->attribute.string);
		act->token->attribute.string = NULL;
	}
	free(act->token);		
	free(act);
}
