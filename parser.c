#include "parser.h"

int err_code = 0;

bool r_program()
{

	if ( (token == Scope) || (token == Declare) || (token == Function))
	{
		/* Simulacia pravidla '1' */
		if ( r_fc_dec() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token != KWD_scope )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( r_stat_list() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		if ( token != KWD_end )
			return false;
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( token != KWD_scope )
			return false;
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( token != EOL )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token != TKN_EOF )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
				
	}
	else
		/* syntakticka chyba */ 
		return false;

	return true;
}


bool r_fc_dec()
{
	if ( token == KWD_declare )
	{
		/* Simulacia pravidla '2' */
		
		if ( r_declaration() == false )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		if ( token != EOL )
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
	else if ( token == KWD_function )
	{
		/* Simulacia pravidla '3' */

		if ( r_definition() == false )		
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		if ( token != EOL )	
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
	else if ( token == KWD_scope )
	{
		/* Simulacia pravidla '4' 
		 * epsilon pravidlo
		 */
		
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


bool r_declaration()
{
	if ( token == KWD_declare )
	{
		/* Simulacia pravidla '5' */
	
		token = get_token();

		if ( token != KWD_function )	
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token != ID )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token != left_par )
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

		if ( token != right_par )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( token != KWD_as )
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

		if ( token != EOL )
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

		if ( token != KWD_end )
		{
			//err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token != KWD_function )
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

}


bool r_definition();


bool r_var_declaration();


bool r_var_definition();


bool r_assign();


bool r_expr_list();


bool r_item_list();


bool r_item2_list();


bool r_par_list();


bool r_par2_list();


bool r_rhs();


bool r_stat();


bool r_stat_list();


bool r_type();
