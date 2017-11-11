#include "parser.h"
#include "precedSA.h"

int err_code = 0;

Ttoken *token;

// TODO viac znakov EOL za sebou, return v maine nemoze byt

Ttoken *get_token()
{
	static int i = -1;
	Ttoken *pole[30];

	i++;

	Ttoken *token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_scope;
	pole[0] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[1] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_if;
	pole[2] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_int;
	pole[3] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_then;
	pole[4] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[5] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_else;
	pole[6] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[7] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_end;
	pole[8] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_if;
	pole[9] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[10] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_end;
	pole[11] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_scope;
	pole[12] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[13] = token;	

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOF;
	pole[14] = token;
/*
	printf("pole[%d] = %d\n", i, pole[i]->type );*/
	return pole[i];
}





bool r_program()
{

	if ( (token->type == KWD_scope) || (token->type == KWD_declare) || 
	(token->type == KWD_function) )
	{
		/* Simulacia pravidla '1' */

		if ( r_fc_dec() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_scope )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();

		if ( token->type != TKN_EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_end )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
	
		if ( token->type != KWD_scope )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( token->type != TKN_EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_EOF )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
				
	}
	else
	{
		//err_code = SYNTAX_ERR;
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
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();

		if ( r_fc_dec() == false )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

	}
	else if ( token->type == KWD_function )
	{
		/* Simulacia pravidla '3' */

		if ( r_definition() == false )		
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		if ( token->type != TKN_EOL )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_fc_dec() == false )
		{
			//err_code = SYNTAX_ERR;
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

		//err_code = SYNTAX_ERR;
		return false;
	}
	return true;
}
/* KONIEC r_fc_dec() */


bool r_declaration()
{
	if ( token->type == KWD_declare )
	{
		/* Simulacia pravidla '5' */
	
		token = get_token();

		if ( token->type != KWD_function )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_id )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_leftpar )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( r_item_list() == false )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_rightpar )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( token->type != KWD_as )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_type() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		/* Syntakticka chyba */
		//err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_declaration */


bool r_definition()
{
	/* Simulacia pravidla '6' */	

	if ( token->type != KWD_function  )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( token->type != TKN_id )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( token->type != TKN_leftpar )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( r_item_list() == false )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	if ( token->type != TKN_rightpar )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( token->type != KWD_as )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( r_type() == false )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	if ( token->type != TKN_EOL )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( r_stat_list == false )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	if ( token->type != KWD_end )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	token = get_token();

	if ( token->type != KWD_function )
	{
		//err_code = SYNTAX_ERR;
		return false;
	}
	
	token = get_token();

	return true;
}
/* KONIEC r_definition() */


bool r_var_declaration()
{
	if ( token->type == KWD_dim )
	{
		/* Simulacia pravidla '7' */

		token = get_token();

		if ( token->type != TKN_id )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != KWD_as )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_type() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( r_var_definition() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	return true;	
}
/* KONIEC r_var_declaration() */


bool r_var_definition()
{
	if ( token->type == TKN_eq )
	{
		/* Simulacia pravidla '9' */
	
		if ( vyhodnot_vyraz(&token) != 0) 
		{
			/* TODO err code */
			//err_code = SYNTAX_ERR;
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
		//err_code = SYNTAX_ERR;
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
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_rhs() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
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
	
		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			//err_code = SYNTAX_ERR;
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
		//err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_expr_list() */


bool r_item_list()
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
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_type() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( r_item2_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else 
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	return true;	
}
/* KONIEC r_item_list() */


bool r_item2_list()
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
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != KWD_as )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( r_type() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( r_item2_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
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
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
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
		//err_code = SYNTAX_ERR;
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
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( r_par2_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	return true;
}
/* KONIEC r_par2_list() */


bool r_rhs()
{
	if ( token->type == TKN_id )
	{
		/* TODO Rozhodnut ci je to premenna alebo volanie funkcie */
	}
	else if ( (token->type == TKN_leftpar) || ( token->type == TKN_int ) ||
	(token->type == TKN_id ) || ( token->type == TKN_dbl ) || 
	( token->type == TKN_str ) )
	{
		/* Simulacia pravidla '20' */

		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
		return false;
	}
	return true;
}
/* KONIEC r_rhs() */


bool r_stat()		/* TODO otestovat */
{
	if ( token->type == KWD_dim )
	{
		/* Simulacia pravidla '28' */

		if ( r_var_declaration() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '27' */

		if ( r_assign() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}	
	}
	else if ( token->type == KWD_input )
	{
		/* Simulacia pravidla '22' */

		token = get_token();

		if ( token->type != TKN_id )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();

		if ( token->type != TKN_EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
	}
	else if ( token->type == KWD_print )
	{
		/* Simulacia pravidla '23' */
		
		token = get_token();
	
		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_smcolon )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_expr_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else if ( token->type == KWD_if )
	{
		/* Simulacia pravidla '24' */

		token = get_token();

		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			//err_code = SYNTAX_ERR;
			return false;
		}

		
		if ( token->type != KWD_then )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != TKN_EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( r_stat_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_else )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token->type != TKN_EOL ) 
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
			
		if ( token->type != KWD_end )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token->type != KWD_if)
		{
			//err_code = SYNTAX_ERR;
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
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();
	
		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != KWD_loop )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

	}
	else if ( token->type == KWD_return )
	{
		/* Simulacia pravidla '26' */

		/* TODO iba vo funkcii*/
		if ( vyhodnot_vyraz(&token) != 0 )
		{
			/* TODO err_code */
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

return true;
}
/* KONIEC r_stat() */


bool r_stat_list()
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
		
		if ( r_stat() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( r_stat_list() == false )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	}
	else
	{
		//err_code = SYNTAX_ERR;
		return false;
	}

	#endif

	/* Simulacia pravidla '31' */

	else
	{
		if ( r_stat() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
	
		token = get_token();
			
		if ( r_stat_list() == false )
		{
			//err_code = SYNTAX_ERR;
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
		//err_code = SYNTAX_ERR;
		return false;
	}
}

/* KONIEC r_type */

int main()
{
	token = get_token();
	
	if(r_program() == true)
		printf("SYNTAX OK\n");
	else
		printf("SYNTAX ERROR\n");

	return 0;
}
