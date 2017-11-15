#include "parser.h"


int err_code = 0;
SYMTB_itemptr_g global_symtb = NULL;

Ttoken *token;

bool in_scope = false;

/* TODO viac znakov EOL za sebou, return v maine nemoze byt,
 * skontrolovat ci boli vsetky deklarovane funkcie aj definovane 
 * funkcia musi byt deklarovana MAX raz, deklaracia musi byt pred definiciou
 * implicitna inicializacia premennych
 * kazda premenna musi byt definovana
 */


bool r_program()
{

	if ( (token->type == KWD_scope) || (token->type == KWD_declare) || 
	(token->type == KWD_function) )
	{
		/* Simulacia pravidla '1' */

		if ( r_fc_dec() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_scope )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		/* Kontrola ci vsetky deklarovane funkcie boli aj definovane */
		
		if ( check_functions(global_symtb) == false )
		{
			err_code = SEMANT_ERR;
			fprintf(stderr, "Nie vsetky deklarovane funkcie boli definovane\n");
			return false;
		}
		in_scope = true;

		token = get_token();

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		/* TODO */

		SYMTB_itemptr_l scope_symtb = NULL;

		if ( r_stat_list(&scope_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_end )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
	
		if ( token->type != KWD_scope )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_EOF )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
				
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_program() */


bool r_fc_dec()
{
	if ( token->type == KWD_declare )
	{
		/* Simulacia pravidla '2' */
		
		if ( r_declaration() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();

		if ( r_fc_dec() == false )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

	}
	else if ( token->type == KWD_function )
	{
		/* Simulacia pravidla '3' */

		if ( r_definition() == false )		
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		if ( token->type != TKN_EOL )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_fc_dec() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

	}
	else if ( token->type == KWD_scope )
	{
		/* Simulacia pravidla '4' */
		  
		/* epsilon pravidlo */
		
		return true;
	}
	else
	{
		/* Syntakticka chyba */

		err_code = SYNTAX_ERR;
		return false;
	}
	return true;
}
/* KONIEC r_fc_dec() */


bool r_declaration()
{
	SYMTB_itemptr_g function = NULL;	
	char *fc_name = NULL;

	if ( token->type == KWD_declare )
	{
		/* Simulacia pravidla '5' */
	
		token = get_token();

		if ( token->type != KWD_function )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_id )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		fc_name = token->attribute.string;
		function = GST_add_function(&global_symtb, fc_name, true, false, 0, "");

		if( function == NULL )
		{
			/* Chyba alokacie */
			return false;
		}
		/* Funkcia s rovnakym menom uz je v tabulke symbolov */
		else if (err_code == -1)
		{
			if (function->fc_declared == true)
			{
				/* Opatovne deklarovanie funkcie */
				fprintf(stderr,"Opatovne deklarovanie funkcie\n");
				err_code = SEMANT_ERR;
				return false;
			}
			else
			{
				/* Definicia funkcie predchadza jej deklaraciu */
				fprintf(stderr, "Definicia funkcie predchadza jej deklaraciu\n");
				err_code = SEMANT_ERR;
				return false;
			}
		}

		token = get_token();

		if ( token->type != TKN_leftpar )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( r_item_list(function) == false )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_rightpar )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		Ttoken *tmp = token;

		if ( r_type() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		/* Pridanie navratoveho typu */
		function->ret_type = type2char(tmp);
	}
	else
	{
		/* Syntakticka chyba */
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_declaration */


bool r_definition()
{
	/* Simulacia pravidla '6' */	

	SYMTB_itemptr_g function = NULL;
	char *fc_name = NULL;
	char *params = NULL;

	if ( token->type != KWD_function  )
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( token->type != TKN_id )
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	fc_name = token->attribute.string;
	function = GST_add_function(&global_symtb, fc_name, false, true, 0, "");

	if ( function == NULL )
	{
		/* Chyba alokacie */
		return false;
	}
	/* Funkcia s rovnakym menom uz je v tabulke symbolov */
	else if (err_code == -1)
	{
		if (function->fc_defined == true)
		{
			/* Opatovne definovanie funkcie */
			fprintf(stderr,"Opatovne definovanie funkcie\n");
			err_code = SEMANT_ERR;
			return false;
		}
		else
		{
			/* Uz bola deklarovana ale nie definovana */

			params = malloc((function->par_count+1) * sizeof(char));
			strcpy(params,function->parameters);
			function->par_count = 0;
		}
	}

	token = get_token();

	if ( token->type != TKN_leftpar )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	if ( r_item_list(function) == false )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	if ( token->type != TKN_rightpar )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	if ( token->type != KWD_as )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	Ttoken *tmp = token;

	if ( r_type() == false )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	if ( function->fc_declared == false)
	{
		/* Pridanie navratoveho typu */
		function->ret_type = type2char(tmp);
	}
	else
	{
		/*  Kontrola ci sa zhoduju navratove typy deklaracie a definicie */
		if ( function->ret_type != type2char(tmp) )
		{
			err_code = SEMANT_ERR;
			fprintf(stderr, "Navratovy typ funkcie v deklaracii a definicii sa nezhoduje\n");
			if( params != NULL )
				free(params);
			
			return false;
		}
	}

	if ( token->type != TKN_EOL )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	if ( r_stat_list(&(function->local_symtb)) == false )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	if ( token->type != KWD_end )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	if ( token->type != KWD_function )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}
	
	token = get_token();

	if ( params != NULL )
	{
		if ( strcmp(params, function->parameters) != 0 )
		{
			/* Typy parametrov v deklaracii a definicii sa nezhoduju */
			fprintf(stderr, "Typy parametrov v deklaracii a definicii sa nezhoduju\n");
			err_code = SEMANT_ERR;
			free(params);
			return false;
		}
		else
		{
			free(params);
		}
	}
	
	return true;
}
/* KONIEC r_definition() */


bool r_var_declaration(SYMTB_itemptr_l *local_symtb)
{
	SYMTB_itemptr_l var = NULL;
	char *var_name = NULL;

	if ( token->type == KWD_dim )
	{
		/* Simulacia pravidla '7' */

		token = get_token();

		if ( token->type != TKN_id )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		var_name = token->attribute.string;
		if ( GST_search(global_symtb, var_name) != NULL )
		{
			/* Uz existuje funkcia s tymto menom */
			fprintf(stderr, "Uz existuje funkcia s nazvom ktorym chete pomenovat premennu\n");
			err_code = SEMANT_ERR;
			return false;

		}	
		
		var = LST_add_var(local_symtb, var_name, 0);

		if ( var == NULL )
		{
			/* Chyba alokacie */
			fprintf(stderr, "Chyba alokacie pamati\n");
			return false;
		}
		else if ( err_code == -1 )
		{
			err_code = SEMANT_ERR;
			fprintf(stderr, "Opatovna definicia premennej\n");
			return false;	
		}

		token = get_token();
		
		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		Ttoken *tmp = token;

		if ( r_type() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		/* Pridanie typu premennej a implicitna inicializacia */

		var->type = type2char(tmp);

		switch(var->type)
		{
			case 'i' :
				var->value.int_value = 0;
				break;
			case 'd' :
				var->value.dbl_value = 0.0;
				break;
			case 's' :
				var->value.str_value = malloc(sizeof(char));
				strcpy(var->value.str_value, "");
				break;
		}
		if ( r_var_definition(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;	
}
/* KONIEC r_var_declaration() */


bool r_var_definition(SYMTB_itemptr_l *local_symtb)
{

	/* TODO priradit hodnotu a skontrolovat typy oboch stran */

	if ( token->type == TKN_eq )
	{
		/* Simulacia pravidla '9' */
	
		if ( (err_code = vyhodnot_vyraz(&token)) != 0) 
		{
			return false;
		}
	}
	else if ( token->type == TKN_EOL )
	{
		/* Simulacia pravidla '35' */
		/* Epsilon pravidlo */

		return true;
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_var_definition() */


bool r_assign() 		/* TODO otestovat */
{
	if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '8' */

		token = get_token();

		if ( token->type != TKN_eq )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_rhs() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}
	return true;
}
/* KONIEC r_assign() */


bool r_expr_list()
{
	if ( ( token->type == TKN_id ) || ( token->type == TKN_leftpar ) ||
	( token->type == TKN_int ) || ( token->type == TKN_str ) ||
	( token->type == TKN_dbl ) )
	{
		/* Simulacia pravidla '10' */
	
		if ( (err_code = vyhodnot_vyraz(&token)) != 0 )
		{
			return false;
		}		
	}
	else if ( token->type == TKN_EOL)
	{
		/* Simulacia pravidla '11' */
		/* Epsilon pravidlo */

		return true;
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_expr_list() */


bool r_item_list(SYMTB_itemptr_g function)
{
	if ( token->type == TKN_rightpar)
	{
		/* Simulacia pravidla '12' */
		/* Epsilon pravidlo */

		return true;	
	}
	else if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '13' */

		token = get_token();

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		Ttoken *param = token;

		if ( r_type() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		GST_add_par(function, type2char(param));

		if ( r_item2_list(function) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else 
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;	
}
/* KONIEC r_item_list() */


bool r_item2_list(SYMTB_itemptr_g function)
{
	
	if ( token->type == TKN_rightpar)
	{
		/* Simulacia pravidla '14' */
		/* Epsilon pravidlo */

		return true;	
	}
	else if ( token->type == TKN_colon ) 
	{
		/* Simulacia pravidla '15' */

		token = get_token();
		if ( token->type != TKN_id )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		Ttoken *param = token;

		if ( r_type() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		GST_add_par(function, type2char(param));

		if ( r_item2_list(function) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_item2_list() */


bool r_par_list()
{
	if ( token->type == TKN_rightpar )
	{
		/* Simulacia pravidla '16' */
		/* Epsilon pravidlo */

		return true;
	}
	else if ( r_par_par() == true )
	{
		/* Simulacia pravidla '17' */

		if ( r_par2_list() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;		
}
/* KONIEC r_par_list() */


bool r_par_par()
{
	if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '36' */
		
		token = get_token();
		return true;
	}
	else if ( token->type == TKN_int )
	{
		/* Simulacia pravidla '37' */
		
		token = get_token();
		return true;
	}
	else if ( token->type == TKN_dbl )
	{
		/* Simulacia pravidla '38' */
		
		token = get_token();
		return true;
	}
	else if ( token->type == TKN_str )
	{
		/* Simulacia pravidla '39' */
		
		token = get_token();
		return true;
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}
	return true;
}
/* KONIEC r_par_par() */


bool r_par2_list()
{
	if ( token->type == TKN_rightpar )
	{
		/* Simulacia pravidla '18' */
		/* Epsilon pravidlo */

		return true;
	}
	else if ( token->type = TKN_colon )
	{
		/* Simulacia pravidla '19' */

		token = get_token();

		if ( r_par_par() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( r_par2_list() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_par2_list() */


bool r_rhs()
{
	if ( token->type == TKN_id )
	{
		/* TODO Rozhodnut ci je to premenna alebo volanie funkcie 
		 * skontrolovat typy parametrov vo volani funkcie
		 */
		
	}
	else if ( (token->type == TKN_leftpar) || ( token->type == TKN_int ) ||
	(token->type == TKN_id ) || ( token->type == TKN_dbl ) || 
	( token->type == TKN_str ) )
	{
		/* Simulacia pravidla '20' */

		if ( (err_code = vyhodnot_vyraz(&token) != 0 ))
		{
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}
	return true;
}
/* KONIEC r_rhs() */


bool r_stat(SYMTB_itemptr_l *local_symtb)		/* TODO otestovat */
{
	if ( token->type == KWD_dim )
	{
		/* Simulacia pravidla '28' */

		if ( r_var_declaration(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '27' */

		if ( r_assign() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}	
	}
	else if ( token->type == KWD_input )
	{
		/* Simulacia pravidla '22' */

		token = get_token();

		if ( token->type != TKN_id )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();
	}
	else if ( token->type == KWD_print )
	{
		/* Simulacia pravidla '23' */
		
		token = get_token();
	
		if ( (err_code = vyhodnot_vyraz(&token)) != 0 )
		{
			return false;
		}

		if ( token->type != TKN_smcolon )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_expr_list() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else if ( token->type == KWD_if )
	{
		/* Simulacia pravidla '24' */

		token = get_token();

		if ( (err_code = vyhodnot_vyraz(&token)) != 0 )
		{
			return false;
		}

		
		if ( token->type != KWD_then )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( r_stat_list(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_else )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token->type != TKN_EOL ) 
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
			
		if ( token->type != KWD_end )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != KWD_if)
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();
	}
	else if ( token->type == KWD_do )
	{
		/* Simulacia pravidla '25' */

		token = get_token();

		if ( token->type != KWD_while )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();
	
		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_loop )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

	}
	else if ( token->type == KWD_return )
	{
		/* Simulacia pravidla '26' */

		if ( in_scope == true )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( (err_code = vyhodnot_vyraz(&token)) != 0 )
		{
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

return true;
}
/* KONIEC r_stat() */


bool r_stat_list(SYMTB_itemptr_l *local_symtb)
{
	if ( (token->type == KWD_end) || 
	(token->type == KWD_loop) || (token->type == KWD_else))
	{
		/* Simulacia pravidla '30' */
		/* Epsilon pravidlo */
		return true;
	}

	#if 0

	else if ( (token->type == KWD_input) || (token->type == KWD_print) ||
	(token->type == KWD_if) || (token->type == KWD_do) || 
	(token->type == KWD_return) || (token->type == TKN_id) || 
	(token->type == KWD_dim) || (token->type == TKN_int) ||
	(token->type == TKN_dbl) || (token->type == TKN_str) )
	{
		/* Simulacia pravidla '31' */
		
		if ( r_stat(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list(local_symtb) == false )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	#endif

	/* Simulacia pravidla '31' */

	else
	{
		if ( r_stat(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();
			
		if ( r_stat_list(local_symtb) == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
	}
	return true;
}
/* KONIEC r_stat_list() */


bool r_type()
{
	if ( token->type == KWD_integer )
	{
		/* Simulacia pravidla '32' */
		
		token = get_token();
		return true;
	}
	else if ( token->type == KWD_double )
	{
		/* Simulacia pravidla '33' */

		token = get_token();
		return true;
	}
	else if ( token->type == KWD_string )
	{
		/* Simulacia pravidla '34' */

		token = get_token();
		return true;
	}
	else 
	{
		err_code = SYNTAX_ERR;
		return false;
	}
}


char type2char(Ttoken *token)
{
	if (token->type == KWD_integer)
		return 'i';
	else if (token->type == KWD_double)
		return 'd';
	else 
		return 's';
}

/* KONIEC r_type */


bool check_functions(SYMTB_itemptr_g RootPtr)
{
	if ( RootPtr != NULL )
	{
		if ( (RootPtr->fc_declared == true) && (RootPtr->fc_defined == false) )
			return false;
		else
			return (true && check_functions(RootPtr->rptr) && check_functions(RootPtr->lptr));

	}
	return true;
}


int main()
{

	SYMTB_itemptr_l local = NULL;
	SYMTB_itemptr_l tmp = NULL;	

	GST_add_function(&global_symtb, "hello", true, true, 's', "si");	
	GST_add_function(&global_symtb, "hell", true, true, 's', "si");	
	GST_add_function(&global_symtb, "world", true, true, 's', "si");	
	GST_add_function(&global_symtb, "shiit", false, true, 's', "si");	
	GST_add_function(&global_symtb, "hole", true, true, 's', "si");	
	GST_add_function(&global_symtb, "jesus", true, true, 's', "si");	
	GST_add_function(&global_symtb, "christ", true, true, 's', "si");	
	GST_add_function(&global_symtb, "morning", true, false, 's', "si");	
	GST_add_function(&global_symtb, "halo", true, true, 's', "si");	
	
	Print_tree_g(global_symtb);	
	
	if ( check_functions(global_symtb) == true )
		printf("Fce OK\n");
	else
		printf("NOT OK");

//	LST_add_var(&local, "hello", 'i');

	//token = get_token();
		
	/*if(r_var_declaration(&local) == true)
		printf("OK\n");
	else
	{
		if (err_code == SYNTAX_ERR)
			printf("SYNTAX ERROR\n");
		else if (err_code == SEMANT_ERR)
			printf("SEMANTIC ERROR\n");
	}

	Print_tree_l(local);

	GST_add_function(&global_symtb, "hello", true, false, 's', "si");	

	token = get_token();
	
	if(r_definition() == true)
		printf("OK\n");
	else
	{
		if (err_code == 0)
			printf("SYNTAX ERROR\n");
		else if (err_code = SEMANT_ERR)
			printf("SEMANTIC ERROR\n");
	}

	Print_tree_g(global_symtb);*/


	return 0;
}
