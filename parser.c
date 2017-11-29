/**
 * Názov: parser.c
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include "parser.h"


int err_code = 0;						/* kod chyby, 0 ak bez chyby */
SYMTB_itemptr_g global_symtb = NULL;	/* globalna tabulka symbolov */
SYMTB_itemptr_l scope_symtb = NULL;		/* tabulka symbolov pre hlavne telo programu */
instruction_lst list;					/* paska instrukcii */

Ttoken *token;

SYMTB_itemptr_g actual_function = NULL;
char ins_typ = 0;						/* typ vysledku precedencnej */
bool in_scope = false;
int par_counter = 0;
unsigned if_counter = 0;
unsigned while_counter = 0;

bool r_program()
{

	if ( skip_EOL() == false )
	{
		// TODO
		return false;
	}

	if ( (token->type == KWD_scope) || (token->type == KWD_declare) || 
	(token->type == KWD_function) )
	{
		/* Simulacia pravidla '1' */

		add_instruction(&list, ".ifjcode17");
		add_instruction(&list, "jump main");
		build_all();		

		if ( r_fc_dec() == false )
		{
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
	
		if ( token == NULL )
		{
			// TODO
			return false;
		}
	
		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( generate_main(&list) == false )
		{
			return false;
		}
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}


		if ( r_stat_list(&scope_symtb) == false )
		{
			return false;
		}


		if ( token->type != KWD_end )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}
	
		if ( token->type != KWD_scope )
		{
		printf("token = %d\n", token->type);
			err_code = SYNTAX_ERR;
			return false;
		}
		
		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		/*if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}*/

		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

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
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_fc_dec() == false )	
		{
			return false;
		}

	}
	else if ( token->type == KWD_function )
	{
		/* Simulacia pravidla '3' */

		if ( r_definition() == false )		
		{
			return false;
		}
		
		if ( token->type != TKN_EOL )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_fc_dec() == false )
		{
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

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != KWD_function )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

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

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != TKN_leftpar )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_item_list(function, false) == false )	
		{
			return false;
		}

		if ( token->type != TKN_rightpar )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

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

	if ( token == NULL )
	{
		// TODO
		return false;
	}

	if ( token->type != TKN_id )
	{
		err_code = SYNTAX_ERR;
		return false;
	}

	fc_name = token->attribute.string;
	function = GST_add_function(&global_symtb, fc_name, false, false, 0, "");
	actual_function = function;	

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
			strcpy(params,function->parameters); /* Kvoli kontrole typov parametrov */
			function->par_count = 0;
			err_code = 0;
		}
	}
	
	char *instr = malloc((strlen(fc_name) + 7) * sizeof(char));	/* 7 -- strlen("label \0") */
	strcpy(instr, "label ");
	strcat(instr, fc_name);
	add_instruction(&list, instr);
	free(instr);

	add_instruction(&list, "defvar lf@return");
	add_instruction(&list, "defvar lf@$result");

	token = get_token();

	if ( token == NULL )
	{
		// TODO
		return false;
	}

	if ( token->type != TKN_leftpar )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}
	
	token = get_token();

	if ( token == NULL )
	{
		// TODO
		return false;
	}
	

	if ( r_item_list(function, true) == false )
	{
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

	if ( token == NULL )
	{
		// TODO
		return false;
	}

	if ( token->type != KWD_as )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	if ( token == NULL )
	{
		// TODO
		return false;
	}

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

	/* navratova hodnota funkcie */
	switch(function->ret_type)
	{
		case 'i' :
			add_instruction(&list, "move lf@return int@0");
			break;
		case 'd' :
			add_instruction(&list, "move lf@return float@0.0");
			break;
		case 's' :
			add_instruction(&list, "move lf@return string@");
			break;
	}

	if ( token->type != TKN_EOL )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	if ( skip_EOL() == false )
	{
		// TODO
		return false;
	}

	if ( token == NULL )
	{
		// TODO
		return false;
	}

	if ( r_stat_list(&(function->local_symtb)) == false )
	{
		if ( params != NULL )
			free(params);
		return false;
	}

	function->fc_defined = true;

	if ( token->type != KWD_end )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}

	token = get_token();

	if ( token == NULL )
	{
		// TODO
		return false;
	}

	if ( token->type != KWD_function )
	{
		err_code = SYNTAX_ERR;
		if ( params != NULL )
			free(params);
		return false;
	}
	
	token = get_token();

	if ( token == NULL )
	{
		// TODO
		return false;
	}

	if ( params != NULL )
	{
		if ( strcmp(params, function->parameters) != 0 )
		{
			/* Parametre v deklaracii a definicii sa nezhoduju */
			fprintf(stderr, "Parametre v deklaracii a definicii funkcie sa nezhoduju\n");
			err_code = SEMANT_ERR;
			free(params);
			return false;
		}
		else
		{
			free(params);
		}
	}

	add_instruction(&list, "return");
	
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

		if ( token == NULL )
		{
			// TODO
			return false;
		}

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
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		/* Instrukcie pre definovanie premennej */
		char instr[1000];
		strcpy(instr, "defvar lf@");
		strcat(instr, var_name);
		add_instruction(&list, instr);

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

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
				sprintf(instr, "move lf@%s int@0", var_name);
				add_instruction(&list, instr);
				break;
			case 'd' :
				var->value.dbl_value = 0.0;
				sprintf(instr, "move lf@%s float@0.0", var_name);
				add_instruction(&list, instr);
				break;
			case 's' :
				var->value.str_value = malloc(sizeof(char));
				strcpy(var->value.str_value, "");
				sprintf(instr, "move lf@%s string@", var_name);
				add_instruction(&list, instr);
				break;
		}
		if ( r_var_definition(local_symtb, var) == false )
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
/* KONIEC r_var_declaration() */


bool r_var_definition(SYMTB_itemptr_l *local_symtb, SYMTB_itemptr_l var)
{

	/* TODO priradit hodnotu a skontrolovat typy oboch stran */

	if ( token->type == TKN_eq )
	{
		/* Simulacia pravidla '9' */
	
		char *var_name = var->var_name;

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( (err_code = vyhodnot_vyraz(&token, *local_symtb, true)) != 0) 
		{
			return false;
		}

		if ( var->type == 's' )
		{
			if ( ins_typ == 's' )
			{
				char instr[1000];
				sprintf(instr, "move lf@%s lf@$result", var_name);
				add_instruction(&list, instr);
			}
			else
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravne typy pri inicializacii premennej\n");	
				return false;
			}
		}	
		else if ( var->type == 'i' )
		{
			if ( ins_typ == 'i' )
			{
				char instr[1000];
				sprintf(instr, "move lf@%s lf@$result", var_name);	
				add_instruction(&list, instr);
			}
			else if ( ins_typ == 'd' )
			{
				char instr[1000];
				sprintf(instr, "float2r2eint lf@%s lf@$result", var_name);	
				add_instruction(&list, instr);
			}
			else
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravne typy pri inicializacii premennej\n");	
				return false;
			}

		}
		else if ( var->type == 'd' )
		{
			if ( ins_typ == 'i' )
			{
				char instr[1000];
				sprintf(instr, "int2float lf@%s lf@$result", var_name);	
				add_instruction(&list, instr);
			}
			else if ( ins_typ == 'd' )
			{
				char instr[1000];
				sprintf(instr, "move lf@%s lf@$result", var_name);	
				add_instruction(&list, instr);
			}
			else
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravne typy pri inicializacii premennej\n");	
				return false;
			}

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


bool r_assign(SYMTB_itemptr_l local_symtb) 		/* TODO otestovat */
{
	SYMTB_itemptr_l var = NULL;

	if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '8' */

		/* Kontrola ci bola definovana premenna */
	
		if ( (var = LST_search(local_symtb, token->attribute.string)) == NULL)
		{
			err_code = SEMANT_ERR;
			fprintf(stderr,"Nedefinovana premenna\n");
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != TKN_eq )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_rhs(local_symtb, var->type) == false )
		{
			return false;
		}

		/* Kontrola typov oboch stran + pripadne pretypovanie */
	
	
		if ( var->type == 's' )
		{
			if ( ins_typ == 's' )
			{
				char instr[1000];
				sprintf(instr, "move lf@%s lf@$result", var->var_name);
				add_instruction(&list, instr);
			}
			else
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravne typy pri priradeni\n");	
				return false;
			}
		}	
		else if ( var->type == 'i' )
		{
			if ( ins_typ == 'i' )
			{
				char instr[1000];
				sprintf(instr, "move lf@%s lf@$result", var->var_name);	
				add_instruction(&list, instr);
			}
			else if ( ins_typ == 'd' )
			{
				char instr[1000];
				sprintf(instr, "float2r2eint lf@%s lf@$result", var->var_name);	
				add_instruction(&list, instr);
			}
			else
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravne typy pri priradeni\n");	
				return false;
			}

		}
		else if ( var->type == 'd' )
		{
			if ( ins_typ == 'i' )
			{
				char instr[1000];
				sprintf(instr, "int2float lf@%s lf@$result", var->var_name);	
				add_instruction(&list, instr);
			}
			else if ( ins_typ == 'd' )
			{
				char instr[1000];
				sprintf(instr, "move lf@%s lf@$result", var->var_name);	
				add_instruction(&list, instr);
			}
			else
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravne typy pri priradeni\n");	
				return false;
			}
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


bool r_expr_list(SYMTB_itemptr_l local_symtb)
{

	if ( ( token->type == TKN_id ) || ( token->type == TKN_leftpar ) ||
	( token->type == TKN_int ) || ( token->type == TKN_str ) ||
	( token->type == TKN_dbl ) )
	{
		/* Simulacia pravidla '10' */
	
		if ( (err_code = vyhodnot_vyraz(&token,local_symtb, true)) != 0 )
		{
			return false;
		}
		
		add_instruction(&list, "write lf@$result");
	
		if ( token->type != TKN_smcolon )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();		

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_expr_list(local_symtb) == false )
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


bool r_item_list(SYMTB_itemptr_g function, bool definition)
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
		
		SYMTB_itemptr_l var = NULL;

		if (definition == true)
		{
			function->par_names[function->par_count] = token->attribute.string;
			var = LST_add_var(&(function->local_symtb), token->attribute.string, 'i');
			if ( err_code == -1 )
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nazvy parametrov funkcie sa zhoduju\n");
				return false;
			}
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		Ttoken *param = token;

		if ( r_type() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		if ( definition == true )
		{
			var->type = type2char(param);
		}	
	
		GST_add_par(function, type2char(param));

		if ( r_item2_list(function, definition) == false )
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
/* KONIEC r_item_list() */


bool r_item2_list(SYMTB_itemptr_g function, bool definition)
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

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != TKN_id )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		SYMTB_itemptr_l var = NULL;
	
		if (definition == true)
		{
			function->par_names[function->par_count] = token->attribute.string;
			var = LST_add_var(&(function->local_symtb), token->attribute.string, 'i');
			if ( err_code == -1 )
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Nazvy parametrov funkcie sa zhoduju\n");
				return false;
			}
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != KWD_as )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		Ttoken *param = token;

		if ( r_type() == false )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( definition == true )
		{
			var->type = type2char(param);
		}	

		GST_add_par(function, type2char(param));

		if ( r_item2_list(function, definition) == false )
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
/* KONIEC r_item2_list() */


bool r_par_list(SYMTB_itemptr_g function, SYMTB_itemptr_l local)
{
	par_counter = 0;

	add_instruction(&list, "createframe");

	if ( token->type == TKN_rightpar )
	{
		/* Simulacia pravidla '16' */
		/* Epsilon pravidlo */

		return true;
	}
	else if ( r_par_par(function, local) == true )
	{
		/* Simulacia pravidla '17' */

		if ( r_par2_list(function, local) == false )
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	if ( par_counter != function->par_count )
	{
		err_code = SEMANT_ERR;
		fprintf(stderr, "Nespravny pocet parametrov vo volani funkcie\n");
		return false;
	}

	return true;		
}
/* KONIEC r_par_list() */


bool r_par_par(SYMTB_itemptr_g function, SYMTB_itemptr_l local)
{
	char *var_name = function->par_names[par_counter];
	strlen(var_name);
		
	char *instr = malloc((strlen(var_name) + 11) * sizeof(char)); /* 11 -- strlen("defvar tf@\0") */
	sprintf(instr, "defvar tf@%s", var_name);
	add_instruction(&list, instr);
	free(instr);


	if ( par_counter >= function->par_count )
	{
		err_code = SEMANT_ERR;
		fprintf(stderr, "Nespravny pocet parametrov vo volani funkcie\n");
		return false;
	}

	if ( token->type == TKN_id )
	{
		/* Simulacia pravidla '36' */

		/* Kontrola definicie premennej */
	
		SYMTB_itemptr_l var = NULL;	

		if ( (var = LST_search(local, token->attribute.string)) == NULL)
		{
			err_code = SEMANT_ERR;
			fprintf(stderr, "Nedefinovana premenna\n");
			return false;
		}
		
		/* Kontrola typu parametru */

		if ( var->type == function->parameters[par_counter] )
		{
			
			char *var_name = function->par_names[par_counter];
			char instr[1000];
			sprintf(instr, "move tf@%s lf@%s", var_name, var->var_name);	
			add_instruction(&list, instr);
		}
		else if ( function->parameters[par_counter] == 'i' )
		{
			if ( var->type == 's' )
			{	
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravny typ parametru vo volani funkcie\n");
				return false;
			}
			else /* var->type == 'd' */
			{
				char *var_name = function->par_names[par_counter];
				char instr[1000];
				sprintf(instr, "float2r2eint tf@%s lf@%s", var_name, token->attribute.string);	
				add_instruction(&list, instr);
			}
		}
		else if ( function->parameters[par_counter] == 'd' )
		{

			if ( var->type == 's' )
			{	
				err_code = TYPE_ERR;
				fprintf(stderr, "Nespravny typ parametru vo volani funkcie\n");
				return false;
			}
			else /* var->type == 'i' */
			{
				char *var_name = function->par_names[par_counter];
				char instr[1000];
				sprintf(instr, "int2float tf@%s lf@%s", var_name, token->attribute.string);	
				add_instruction(&list, instr);
			}
		}
		else
		{
			err_code = TYPE_ERR;
			fprintf(stderr, "Nespravny typ parametru vo volani funkcie\n");
			return false;
		}
		

		par_counter++;	
	
		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		return true;
	}
	else if ( token->type == TKN_int )
	{
		/* Simulacia pravidla '37' */
		
		/* Kontrola typu parametru */

		if ( function->parameters[par_counter] == 's' )
		{
			err_code = TYPE_ERR;
			fprintf(stderr, "Nespravny typ parametru vo volani funkcie\n");
			return false;
		}
		else if ( function->parameters[par_counter] == 'i' )
		{
			char *var_name = function->par_names[par_counter];
			char instr[1000];
			sprintf(instr, "move tf@%s int@%d", var_name, token->attribute.integer);	
			add_instruction(&list, instr);
		}
		else if ( function->parameters[par_counter] == 'd' )
		{
			char *var_name = function->par_names[par_counter];
			char instr[1000];
			sprintf(instr, "int2float tf@%s int@%d", var_name, token->attribute.integer);	
			add_instruction(&list, instr);
		}

		par_counter++;	

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		return true;
	}
	else if ( token->type == TKN_dbl )
	{
		/* Simulacia pravidla '38' */
		
		/* Kontrola typu parametru + implicitne konverzie */

		if ( function->parameters[par_counter] == 's' )
		{
			err_code = TYPE_ERR;
			fprintf(stderr, "Nespravny typ parametru vo volani funkcie\n");
			return false;
		}
		else if ( function->parameters[par_counter] == 'i' )
		{
			char *var_name = function->par_names[par_counter];
			char instr[1000];
			sprintf(instr, "float2r2eint tf@%s float@%g", var_name, token->attribute.dble);	
			add_instruction(&list, instr);
		}
		else if ( function->parameters[par_counter] == 'd' )
		{
			char *var_name = function->par_names[par_counter];
			char instr[1000];
			sprintf(instr, "move tf@%s float@%g", var_name, token->attribute.dble);	
			add_instruction(&list, instr);
		}


		par_counter++;	

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		return true;
	}
	else if ( token->type == TKN_str )
	{
		/* Simulacia pravidla '39' */
		
		/* Kontrola typu parametru */

		if ( function->parameters[par_counter] != 's' )
		{
			err_code = TYPE_ERR;
			fprintf(stderr, "Nespravny typ parametru vo volani funkcie\n");
			return false;
		}

		char *var_name = function->par_names[par_counter];
		/* 17 - dlzka retazca "move tf@ string@" + ukoncujuca '\0'  */
		int length = (strlen(var_name) + strlen(token->attribute.string) + 17);
		char *instr = malloc(length * sizeof(char));
		sprintf(instr, "move tf@%s string@%s", var_name, token->attribute.string);	
		add_instruction(&list, instr);
		free(instr);

		par_counter++;	

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

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


bool r_par2_list(SYMTB_itemptr_g function, SYMTB_itemptr_l local)
{
	if ( token->type == TKN_rightpar )
	{
		/* Simulacia pravidla '18' */
		/* Epsilon pravidlo */

		return true;
	}
	else if ( token->type == TKN_colon )
	{
		/* Simulacia pravidla '19' */

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_par_par(function, local) == false )
		{
			return false;
		}

		if ( r_par2_list(function, local) == false )
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
/* KONIEC r_par2_list() */


bool r_rhs(SYMTB_itemptr_l local_symtb, char type)
{
	
	if ( (token->type == TKN_id) || (token->type == KWD_length) || (token->type == KWD_asc) ||
	(token->type == KWD_chr) || (token->type == KWD_substr) )
	{

		SYMTB_itemptr_g function = GST_search(global_symtb, token->attribute.string);

		/* TODO Skontrolovat navratovy typ */

		if ( function == NULL)
		{
			goto variable;
		}
		else
		{
			/* Simulacia pravidla '21' */
	
				
			if ( (function->fc_declared == false) && (function->fc_defined == false))
			{
				/* Funkcia nebola deklarovana a rekurzivne vola sama seba */

				fprintf(stderr, "Funkcia vola rekurzivne samu seba a nebola deklarovana\n");
				err_code = SEMANT_ERR;
				return false;
			}
	
			token = get_token();
	
			if ( token == NULL )
			{
				// TODO
				return false;
			}

			if ( token->type != TKN_leftpar )
			{
				err_code = SYNTAX_ERR;
				return false;
			}
	
			token = get_token();

			if ( token == NULL )
			{
				// TODO
				return false;
			}

			if ( r_par_list(function, local_symtb) == false )
			{
				return false;
			}
			
	
			if ( token->type != TKN_rightpar )
			{
				err_code = SYNTAX_ERR;
				return false;
			}

			add_instruction(&list, "pushframe");
			char *fc_name = function->function_name;
					
			char *instr = malloc((6 + strlen(fc_name)) * sizeof(char)); /* 6 - dlzka retazca "call "+'\0'*/
			sprintf(instr, "call %s", fc_name);
			add_instruction(&list, instr);
			free(instr);		

			add_instruction(&list, "popframe");
	
			ins_typ = function->ret_type;

			if ( type == 'i' )
			{
				if ( function->ret_type == 'i' )
				{
					add_instruction(&list, "move lf@$result tf@return");
				}
				else if ( function->ret_type == 'd' )
				{
					add_instruction(&list, "float2r2eint lf@$result tf@return");
					ins_typ = 'i';
				}
				else
				{
					err_code = TYPE_ERR;
					fprintf(stderr, "Nevhodne typy lavej a pravej strany vyrazu\n");
					return false;
				}	

			}
			else if ( type == 'd' )
			{
				if ( function->ret_type == 'd' )
				{
					add_instruction(&list, "move lf@$result tf@return");
				}
				else if ( function->ret_type == 'i' )
				{
					add_instruction(&list, "int2float lf@$result tf@return");
					ins_typ = 'd';
				}
				else
				{
					err_code = TYPE_ERR;
					fprintf(stderr, "Nevhodne typy lavej a pravej strany vyrazu\n");
					return false;
				}	
			}
			else /* type == 's' */
			{
				if ( function->ret_type != 's' )
				{
					err_code = TYPE_ERR;
					fprintf(stderr, "Nevhodne typy lavej a pravej strany vyrazu\n");
					return false;
				}
				else
				{
					add_instruction(&list, "move lf@$result tf@return");
				}
			}

			token = get_token();
			
			if ( token == NULL )
			{
				// TODO
				return false;
			}

		}
	}
	else if ( (token->type == TKN_leftpar) || ( token->type == TKN_int ) ||
	(token->type == TKN_id ) || ( token->type == TKN_dbl ) || 
	( token->type == TKN_str ) )
	{
		/* Simulacia pravidla '20' */

		variable:
		if ( (err_code = vyhodnot_vyraz(&token, local_symtb, true)) != 0 )
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
			return false;
		}
	}
	else if (token->type == TKN_id)
	{
		/* Simulacia pravidla '27' */

		if ( r_assign(*local_symtb) == false )
		{
			return false;
		}	
	}
	else if ( token->type == KWD_input )
	{
		/* Simulacia pravidla '22' */

		token = get_token();
			
		if ( token == NULL )
		{
			// TODO
			return false;
		}


		if ( token->type != TKN_id )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		SYMTB_itemptr_l var = LST_search(*local_symtb, token->attribute.string);
		if ( var == NULL )
		{
			/* Nedefinovana premenna */
			err_code = SEMANT_ERR;
			fprintf(stderr, "Nedefinovana premenna\n");
			return false;
		}
		else
		{
			add_instruction(&list, "write string@?\\032");
			char *instr = NULL;
			switch(var->type)
			{
				case 'i' :
					instr = malloc((strlen(var->var_name) + 13 ) * sizeof(char));
					strcpy(instr, "read lf@");
					strcat(instr, var->var_name);
					strcat(instr, " int");
					add_instruction(&list,instr);
					break;				
				case 'd' :
					instr = malloc((strlen(var->var_name) + 15 ) * sizeof(char));
					strcpy(instr, "read lf@");
					strcat(instr, var->var_name);
					strcat(instr, " float");
					add_instruction(&list,instr);
					break;				
				case 's' :
					instr = malloc((strlen(var->var_name) + 16 ) * sizeof(char));
					strcpy(instr, "read lf@");
					strcat(instr, var->var_name);
					strcat(instr, " string");
					add_instruction(&list,instr);
					break;					
			}
			free(instr);
		}
	
		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

	}
	else if ( token->type == KWD_print )
	{
		/* Simulacia pravidla '23' */
		
		token = get_token();
		if ( token == NULL )
		{
			// TODO
			return false;
		}

	
		if ( (err_code = vyhodnot_vyraz(&token, *local_symtb, true)) != 0 )
		{
			return false;
		}

		add_instruction(&list, "write lf@$result");

		if ( token->type != TKN_smcolon )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_expr_list(*local_symtb) == false )
		{
			return false;
		}
	}
	else if ( token->type == KWD_if )
	{
		/* Simulacia pravidla '24' */

		token = get_token();
	
		int actual_if = if_counter;
		if_counter++;

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( (err_code = vyhodnot_vyraz(&token, *local_symtb, false)) != 0 )
		{
			return false;
		}

		if ( ins_typ == 'b' )
		{
			char instr[100];
			sprintf(instr, "jumpifneq else_%d lf@$result bool@true", actual_if );
			add_instruction(&list, instr);
		}
		else if ( ins_typ == 'i' )
		{
			char instr[100];
			sprintf(instr, "jumpifeq else_%d lf@$result int@0", actual_if);
			add_instruction(&list, instr);
		}	
		else if ( ins_typ == 'd' )
		{
			char instr[100];
			sprintf(instr, "jumpifeq else_%d lf@$result float@0.0", actual_if);
			add_instruction(&list, instr);
		}	
		else if ( ins_typ == 's' )
		{
			char instr[100];
			sprintf(instr, "jumpifeq else_%d lf@$result string@", actual_if);
			add_instruction(&list, instr);
		}	

		
		if ( token->type != KWD_then )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		
		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_stat_list(local_symtb) == false )
		{
			return false;
		}

		if ( token->type != KWD_else )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != TKN_EOL ) 
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		char jmp_end[100];
		sprintf(jmp_end, "jump endif_%d", actual_if );
		add_instruction(&list, jmp_end);

		char else_if[100];
		sprintf(else_if, "label else_%d", actual_if );
		add_instruction(&list, else_if);

		if ( r_stat_list(local_symtb) == false )
		{
			return false;
		}
		
		char end_if[100];
		sprintf(end_if, "label endif_%d", actual_if );
		add_instruction(&list, end_if);
		
		if ( token->type != KWD_end )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != KWD_if)
		{
			err_code = SYNTAX_ERR;
			return false;
		}
		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

	}
	else if ( token->type == KWD_do )
	{
		/* Simulacia pravidla '25' */

		int actual_while = while_counter;
		while_counter++;

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( token->type != KWD_while )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
			
		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		char ord_while[100];
		sprintf(ord_while, "while_%d", actual_while);
		char while_label[100];
		sprintf(while_label, "label %s",ord_while);	

		add_instruction(&list, while_label);

		if ( (err_code = vyhodnot_vyraz(&token, *local_symtb, false)) != 0 )
		{
			return false;
		}
	
		if ( token->type != TKN_EOL )	
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( ins_typ == 'b' )
		{
			char instr[100];
			sprintf(instr, "jumpifneq end_%s lf@$result bool@true", ord_while);
			add_instruction(&list, instr);
		}
		else if ( ins_typ == 'i' )
		{
			char instr[100];
			sprintf(instr, "jumpifeq end_%s lf@$result int@0", ord_while);
			add_instruction(&list, instr);
		}	
		else if ( ins_typ == 'd' )
		{
			char instr[100];
			sprintf(instr, "jumpifeq end_%s lf@$result float@0.0", ord_while);
			add_instruction(&list, instr);
		}	
		else if ( ins_typ == 's' )
		{
			char instr[100];
			sprintf(instr, "jumpifeq end_%s lf@$result string@", ord_while);
			add_instruction(&list, instr);
		}	

		if ( r_stat_list(local_symtb) == false )
		{
			return false;
		}

		if ( token->type != KWD_loop )
		{
			err_code = SYNTAX_ERR;
			return false;
		}

		token = get_token();	

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		char jump_label[100];
		sprintf(jump_label, "jump %s", ord_while);
		add_instruction(&list, jump_label);
			
		char end_while_label[100];
		sprintf(end_while_label, "label end_%s",ord_while);	
		add_instruction(&list, end_while_label);
	}
	else if ( token->type == KWD_return )
	{
		/* Simulacia pravidla '26' */

		token = get_token();

		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( in_scope == true )
		{
			err_code = SYNTAX_ERR;
			fprintf(stderr, "Return sa nesmie nachadzat v hlavnom tele programu\n");
			return false;
		}

		if ( (err_code = vyhodnot_vyraz(&token, *local_symtb, true)) != 0 )
		{
			return false;
		}

		if ( actual_function->ret_type == 's' )
		{
			if ( ins_typ != 's' )
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Navratovy typ funkcie sa nezhoduje s typom vracanej hodnoty\n");
				return false;
			}
			else
			{
				add_instruction(&list,"move lf@return lf@$result");
			}
		}
		else if ( actual_function->ret_type == 'i' )	
		{
			if ( ins_typ == 's' )
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Navratovy typ funkcie sa nezhoduje s typom vracanej hodnoty\n");
				return false;
			}
			else if ( ins_typ == 'i' )
			{
				add_instruction(&list,"move lf@return lf@$result");
			}
			else /* ins_typ = 'd' */
			{
				add_instruction(&list,"float2r2eint lf@return lf@$result");	
			}
		}
		else	/* 'd' */ 
		{
			if ( ins_typ == 's' )
			{
				err_code = TYPE_ERR;
				fprintf(stderr, "Navratovy typ funkcie sa nezhoduje s typom vracanej hodnoty\n");
				return false;
			}
			else if ( ins_typ == 'i' )
			{
				add_instruction(&list,"int2float lf@return lf@$result");
			}
			else /* ins_typ = 'd' */
			{
				add_instruction(&list,"move lf@return lf@$result");	
			}

		}
		add_instruction(&list, "return"); 
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
		/* Simulacia pravidla '30' 
		 * Epsilon pravidlo */

		return true;
	}


	/* Simulacia pravidla '31' */

	else
	{
		if ( r_stat(local_symtb) == false )
		{
			return false;
		}

		if ( token->type != TKN_EOL )
		{
			err_code = SYNTAX_ERR;
			return false;
		}
	
		if ( skip_EOL() == false )
		{
			// TODO
			return false;
		}	
			
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		if ( r_stat_list(local_symtb) == false )
		{
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
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		return true;
	}
	else if ( token->type == KWD_double )
	{
		/* Simulacia pravidla '33' */

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

		return true;
	}
	else if ( token->type == KWD_string )
	{
		/* Simulacia pravidla '34' */

		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}

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


bool skip_EOL()
{
	while(token->type == TKN_EOL)
	{
		token = get_token();
		
		if ( token == NULL )
		{
			// TODO
			return false;
		}
	}
	return true;
}

int main()
{

	SYMTB_itemptr_l local = NULL;

	GST_add_builtin(&global_symtb);
	init_list(&list);

	token = get_token();

	if ( r_program() == true )
	{
	//	printf("OK\n");
		print_list(list);
		delete_list(&list);
		return 0;
	}
	else
	{
/*		if ( err_code == SEMANT_ERR )
			printf("\n\nSEMANTIC ERROR\n");
		else if ( err_code == SYNTAX_ERR )
			printf("\n\nSYNTAX ERROR\n");
		else if ( err_code == TYPE_ERR )
			printf("\n\nTYPE ERROR\n");
		else
			printf("\n\nOTHER ERROR\n");*/
		delete_list(&list);
		return err_code;
	}


//	Print_tree_g(global_symtb);
//	Print_tree_l(scope_symtb);
	
	return 0;
}
