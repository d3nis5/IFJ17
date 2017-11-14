#include "gettoken.h"

Ttoken *token;

Ttoken *get_token()
{
	static int i = -1;
	Ttoken *pole[30];

	i++;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_function;
	pole[0] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_id;
	token->attribute.string = "hello";
	pole[1] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_leftpar;
	pole[2] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_id;
	pole[3] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_as;
	pole[4] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_string;
	pole[5] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_colon;
	pole[6] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_id;
	pole[7] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_as;
	pole[8] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_integer;
	pole[9] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_rightpar;
	pole[10] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_as;
	pole[11] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_string;
	pole[12] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[13] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_end;
	pole[14] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = KWD_function;
	pole[15] = token;

	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOL;
	pole[16] = token;	
/*
	token = (Ttoken*) malloc(sizeof(Ttoken));
	token->type = TKN_EOF;
	pole[16] = token;

	printf("pole[%d] = %d\n", i, pole[i]->type );*/
	return pole[i];
}
