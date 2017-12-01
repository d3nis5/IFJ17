#include "scanner.h"

char kwds[35][14] = {
	 "as", //idx = 21
	"asc",
	"declare",
 	"dim",
	"do",
	"double",
	"else",
	"end",
	"chr",
	"function",
	"if",
	"input",
	"integer",
	"length",
	"loop",
	"print",
	"return",
	"scope",
	"string",
	"substr",
	"then",
	"while",

	"and",
	"boolean",
	"continue",
	"elseif",
	"exit",
	"false",
	"for",
	"next",
	"not",
	"or",
	"shared",
	"static",
	"true", //idx = 55
};


/* preskoci riadkovy komentar po koniec riadku */
void ignor_ria_kom(int *c) {
	while((*c) != '\n') {
		if ( *c == EOF )
		{
			ungetc(*c, SUBOR);
			return;
		}
		(*c) = fgetc(SUBOR);
	}
}

/* preskoci blokovy komentar po ukoncovaciu sekvenciu */
void ignor_blok_kom(int *c)
{
    int chyba=0;
    *c=fgetc(SUBOR);    //precita dalsi znak
    while(*c != EOF)
    {
        while(*c != '\'')
        {
            *c=fgetc(SUBOR);
            if(*c == EOF)   //error
            {
                chyba=1;
                break;
            }
        }
        if(chyba)
            break;
        *c=fgetc(SUBOR); //cita znak za '
        if(*c == EOF)   //error
        {
            break;  //opusti funkciu
        }
        if (*c == '/')  // /
        {
            break;  //konci komentar
        }
    }
    return;
}

/* prevedie vsetky pismena parametra na male */
char* to_lower_case(char* string) {
	for(int i = 0; string[i]; i++) {
		string[i] = tolower(string[i]);
	}
	return string;
}

/* prevedie string na pozadovany format, pouzitelni instrukciou jazyka IFJ17
** @param string string, ktory treba preformatovat
** @param mem_alloc pamat zatial naalokovana pre string. (v pripade reallocu zvacsena)
*/
char* format_string(char* string, int *mem_alloc) {


	int string_len = strlen(string);
	int num_to_copy = 0;				//pocet bytov na skopirovanie pri posune
	int prva_cif;
	int druha_cif;
	unsigned pamat = *mem_alloc;

	int i = 0;

	//printf("Mem vo format: %d \n", pamat);
	while(string[i]) {


		if((pamat - 1) < (strlen(string) + 5)) {
			pamat = pamat*2;							//zdvojnasob mem_allocated
			string = realloc(string, pamat);					//reallocuj str_pomocny TODO
			//printf("(format)Bola zvacsena alloc. pamat na %d\n", pamat);

		}


		if((string[i] == 92)) {

			if((string[i+1] == '"') || (string[i+1] == 'n') || (string[i+1] == 't') || ((string[i+1]) == 92)) {

				num_to_copy = (string_len - (i + 2));
				memmove(&string[i+5], &string[i+2], num_to_copy);
				string_len += 3;

				if((string[i+1] == '"')) {
					prva_cif = ((string[i+1] /10) + 48); 		//prva cifra ascii hodnoty
					druha_cif = ((string[i+1] %10) + 48);		//druha cifra ascii hodnoty

					string[i+1] = 92;
					string[i+2] = '0';
					string[i+3] = prva_cif;
					string[i+4] = druha_cif;

					i += 5;
					continue;
				}

				if(string[i+1] == 'n') {
					string[i+1] = 92;
					string[i+2] = '0';
					string[i+3] = '1';
					string[i+4] = '0';

					i += 5;
					continue;
				}

				if(string[i+1] == 't') {
					string[i+1] = 92;
					string[i+2] = '0';
					string[i+3] = '0';
					string[i+4] = '9';

					i += 5;
					continue;
				}

				if(string[i+1] == 92) {
					string[i+1] = 92;
					string[i+2] = '0';
					string[i+3] = '9';
					string[i+4] = '2';

					i += 5;
					continue;
				}


			}


			if(isdigit(string[i+1]) && isdigit(string[i+2]) && isdigit(string[i+3])) {


				if(string[i+1] > 50) {
					fprintf(stderr, "Lex error: escape sekvencia mimo rozsah 0-255\n");
					err_code = LEXICAL_ERR;
					return NULL;
				}

				else if((string[i+1] == 50) && ((string[i+2] > 53) || ((string[i+2] == 53) && (string[i+3] > 53) ))) {
					fprintf(stderr, "Lex error: escape sekvencia mimo rozsah 0-255\n");
					err_code = LEXICAL_ERR;
					return NULL;
				}

				else {

					num_to_copy = (string_len - (i + 1));
					memmove(&string[i+2], &string[i+1], num_to_copy);
					string_len += 1;

					i++;
					string[i] = 92;
					i += 4;
					continue;
				}
			}

		}


		if((string[i] <= 32 ) || (string[i] == 35) || (string[i] == 92) ) {


			num_to_copy = (string_len - (i + 1));
			memmove(&string[i+5], &string[i+1], num_to_copy);
			string_len += 5;

			prva_cif = ((string[i] /10) + 48); 		//prva cifra ascii hodnoty
			druha_cif = ((string[i] %10) + 48);		//druha cifra ascii hodnoty

			string[i] = 92;
			string[i+1] = 92;
			string[i+2] = '0';
			string[i+3] = prva_cif;
			string[i+4] = druha_cif;

			i += 5;
			continue;
		}



		i++;
	}
	*mem_alloc = pamat;
	return string;
}

char* reduce_string(char* string) {

	int string_len = strlen(string);
	int num_to_copy = 0;				//pocet bytov na skopirovanie pri posune

	int i = 0;

	while(string[i]) {
		if(string[i] == 92) {
			num_to_copy = (string_len - (i + 1));
			memmove(&string[i], &string[i+1], num_to_copy);
			string[string_len - 1] = '\0';
			string_len -= 1;
			i++;
		}
		i++;
	}

	return string;
}


/* zisti ci je identifikator keyword, ak ano vrati index token type kwd, ak nie vrati 0 cize ID */
int is_kwd(char* ident) {
	for(int i = 0; i < 35; i++) {
		if((strcmp(kwds[i], ident) == 0)) {
			return (i + 21);				//vrati index do TKN_type
		}
	}
	return 0;								//ostane ID
}


Ttoken* get_token() {

	TKN_type stav = -42; //aktualny stav

/*	Ttoken *token = malloc(sizeof(Ttoken));*/
/*	if(token == NULL) {*/
/*		fprintf(stderr, "Nepodarilo sa naalokovat pamat\n");*/
/*		err_code = LEXICAL_ERR;*/
/*	}*/



	Ttoken *token = malloc(sizeof(Ttoken));

	int index = 0; //index k roznym ucelom, treba vzdy nulovat

	char *str_pomocny = NULL;

	int mem_allocated;

	int c = fgetc(SUBOR); //nacitam znaku


	while(!(feof(SUBOR))) { //kym nenarazis na koniec suboru


		while((isspace(c)) && (c != '\n')) { //ak je ine prazdne miesto a stav nie je eol
			c = fgetc(SUBOR); //nacitaj dalsi znak
			//continue;		  // a pokracuj vo vyhodnocovani

		}


		if(c == '\'') { //ak je apostrof, ide o riadkovy koment
			ignor_ria_kom(&c);			//pozn: je nacitany \n. Predat EOL token ci nie ??? ***
		}

		if(c == '\n') {			//ak je c znak konca riadku
			token->type = TKN_EOL;
		}
		else if(c == '+') {
			token->type = TKN_plus;
		}
		else if(c == '-') {
			token->type = TKN_minus;
		}
		else if(c == '*') {
			token->type = TKN_star;
		}
		else if(c == '/') {
			stav = TKN_slash;
		}
		else if(c == '\\') {			//spatne TKN_slash ...
			token->type = TKN_backslash;
		}
		else if(c == '>') {
			stav = TKN_gt;
		}
		else if(c == '<') {
			stav = TKN_less;
		}
		else if(c == '=') {
			token->type = TKN_eq;
		}
		else if(c == ';') {
			token->type = TKN_smcolon;
		}
		else if(c == ',') {
			token->type = TKN_colon;
		}
		else if(c == '(') {
			token->type = TKN_leftpar;
		}
		else if(c == ')') {
			token->type = TKN_rightpar;
		}
		else if(c == '!') {			//String zacina " !" "
			stav = TKN_str;
		}
		else if(isalpha(c) || (c == '_')) {	// _ alebo a-z znaci ID
			stav = TKN_id;
		}
		else if(isdigit(c)) {				//ci je double sa overi az v switchi
			stav = TKN_int;
		}
		else if ( c == EOF ) 
		{
			token->type = TKN_EOF;
			add_token(&token_list, token);
			return token;
		}
		else
		{
			err_code = LEXICAL_ERR;
			return NULL;
		}

		if ( (stav == TKN_id) || (stav == TKN_int) || (stav == TKN_dbl) || (stav == TKN_str) )
			str_pomocny = malloc(ALOC * sizeof(char));

		switch(stav) {


			case TKN_id :
				index = 0;
				mem_allocated = ALOC * sizeof(char);			//alokovana pamat pre pomocny string
				while(isdigit(c) || isalpha(c) || (c == '_')) {

					if((mem_allocated - 1) == index) {
						mem_allocated *= 2;									//zdvojnasob mem_allocated
						str_pomocny = realloc(str_pomocny, mem_allocated);	//reallocuj str_pomocny
					//	printf("Bola zvacsena alloc. pamat na %d\n", mem_allocated);
					}

					str_pomocny[index] = c;
					c = fgetc(SUBOR);
					if ( index > 129 )
					{
						free(str_pomocny);
						err_code = LEXICAL_ERR;
						return NULL;
					}
					index++;
				}
				if (index > 128 )
				{
					free(str_pomocny);
					err_code = LEXICAL_ERR;
					return NULL;
				}
				ungetc(c, SUBOR);
				str_pomocny[index] = '\0';		//ukoncenie retazca

				str_pomocny = to_lower_case(str_pomocny);
				token->type = is_kwd(str_pomocny); 		 //priradi "stav", teda typ tokenu do tokenu

				if ( (token->type == 0) || (token->type == KWD_length) ||
				(token->type == KWD_substr) || (token->type == KWD_asc) ||
				(token->type == KWD_chr) )
					token->attribute.string = str_pomocny;
				else
					free(str_pomocny);

				add_token(&token_list, token);
				return token;	
				
				//printf("ID je : %s \n", token->attribute.string);
				break;


			case TKN_int :
				index = 0;
				while(isdigit(c)) { //ak je znak cislo
					str_pomocny[index] = c; //uloz znak do pomocneho stringu
					c = fgetc(SUBOR); // nacitaj dalsi znak
					index++;
				}

				if(!((c == 'E') || (c == 'e') || (c == '.'))) { 	//ak nejde o double
					ungetc(c, SUBOR); 								//vrat posledny znak
					str_pomocny[index] = '\0'; 						//koniec retazca
					token->type = TKN_int;							//int ostal integer
					token->attribute.integer = atoi(str_pomocny);
					free(str_pomocny);
					//printf("Cele cislo\n");
					break;											// a skonci
				}
				else {	 //TO CHECK ci to ide takto spravit ??
					stav = TKN_dbl;									//znak je bodka -> ide o double
					str_pomocny[index] = c;							//do stringu sa prida bodka
					index++;
				}													//ziaden break -> pokracuje do double.



			case TKN_dbl :	/* v pripade, ze v TKN_int pride bodka	*/

				switch(c) {

					case 'E' :
					case 'e' :
						//printf("cele cislo s exponentom \n");

							c = fgetc(SUBOR);			// nacitaj dalsi znak

							if((c == '+') || (c == '-') || (isdigit(c))) { //ak je tam dobrovolne +/- zapis, ak nie zapis cislo
								str_pomocny[index] = c;
								index++;

							}
							else {
								fprintf(stderr, "Lex Err: po exponente musi ist cislo, alebo +/-");
								err_code = LEXICAL_ERR;
								free(str_pomocny);
								return NULL;
							}

							c = fgetc(SUBOR);

							while(isdigit(c)) {
								str_pomocny[index] = c;
								index++;
								c = fgetc(SUBOR);
							}
					
						ungetc(c, SUBOR);
						break;

					case '.' :
						//printf("Normalny double \n");
						c = fgetc(SUBOR); 				//nacitaj prvy znak po bodke

						if(!(isdigit(c))) {
							fprintf(stderr, "Lexikalna chyba double ma tvar num.num\n");
							err_code = LEXICAL_ERR;
							return NULL;
						}
						else {
							str_pomocny[index] = c;		//zapis cislo do stringu, ktory ma zatial tvar "int."
							index++;
						}

						c = fgetc(SUBOR);

						while(isdigit(c)) {				//zapisuj dalsie cisla za desatinu ciarku
							str_pomocny[index] = c;
							index++;
							c = fgetc(SUBOR);
						}

						if((c == 'E') || (c == 'e')) {	//exponent prisiel za desatinym cislom

							str_pomocny[index] = c;		//zapis do stringu znak e/E
							index++;

							c = fgetc(SUBOR);			// nacitaj dalsi znak

							if((c == '+') || (c == '-') || (isdigit(c))) { //ak je tam dobrovolne +/- zapis, ak nie zapis cislo
								str_pomocny[index] = c;
								index++;

							}
							else {
								fprintf(stderr, "Lex Err: po exponente musi ist cislo, alebo +/-");
								err_code = LEXICAL_ERR;
								return NULL;
							}

							c = fgetc(SUBOR);

							while(isdigit(c)) {
								str_pomocny[index] = c;
								index++;
								c = fgetc(SUBOR);
							}

						}
						ungetc(c, SUBOR);
						break;
				}
				token->type = TKN_dbl;
				token->attribute.dble = strtod(str_pomocny, NULL);
				free(str_pomocny);
				//printf("Double ma hodnotu: %lf \n", token->attribute.dble);

				break;



			case TKN_str :
				index = 0;
				mem_allocated = ALOC * sizeof(char);				//alokovana pamat pre pomocny string

				if((c = fgetc(SUBOR)) == '"') {		//ak zacina str spravne ...

					c = fgetc(SUBOR);

					while(c != '"') { 					//kym neskonci s "

						if((c == '\n') || (c == EOF)) { 	//alebo kym nenarazi na koniec riadku
							fprintf(stderr, "Lex err: string nebol spravne ukonceny s ' \" ' \n");
							err_code = LEXICAL_ERR;
							return NULL;
						}

						if((mem_allocated - 1) == index + 1) {					// index plus 1 z dovodu \"
							mem_allocated *= 2;									//zdvojnasob mem_allocated
							str_pomocny = realloc(str_pomocny, mem_allocated);	//reallocuj str_pomocny
							//printf("Bola zvacsena alloc. pamat na %d\n", mem_allocated);
						}

						str_pomocny[index] = c;									//zapisuj znaky str, do pomocneho str
						c = fgetc(SUBOR);

						if(c == '"') {											//ak narazis na ' " '
							if(str_pomocny[index] == '\\') {					//zisti, ci nejde o pokus zapisat ju...
								index++;
								str_pomocny[index] = '"';
								c = fgetc(SUBOR);
							}
						}

						index++;

					}

					//printf("mem: %d\n", mem_allocated);

					str_pomocny[index] = '\0'; 									//ukonci retazec
					token->type = TKN_str;
					//printf("Povodny string: \"%s\"\n", str_pomocny);
					str_pomocny = format_string(str_pomocny, &mem_allocated);
				
					if ( str_pomocny == NULL )
					{
						err_code = LEXICAL_ERR;
						return NULL;
					}

					str_pomocny = reduce_string(str_pomocny);

					token->attribute.string = str_pomocny;

				}

				else {
					fprintf(stderr, "Chybne zadany string format je : !\" \" \n");
					err_code = LEXICAL_ERR;
					return NULL;
				}
				break;


			case TKN_slash : //moze byt komentar, alebo deleno
				//printf("slash c = %c\n", c );
				if((c = fgetc(SUBOR)) == '\'') {
					ignor_blok_kom(&c);
					stav=-42;
					c=fgetc(SUBOR); 
					continue;
								//
				}
				else { //je to lomitko ...
					//printf("lom c = %c\n", c);
					token->type = TKN_slash;
					ungetc(c, SUBOR);
					break;
				}



			case TKN_less : //moze ostat TKN_lessm, alebo pribudne TKN_eq

				if((c = fgetc(SUBOR)) == '>') { 		//ak za < nasleduje >
					token->type = TKN_neq;
					break;
				}
				else if(c == '=') { 				//ak za < nasleduje = -> '<='
					token->type = TKN_leq;
					break;
				}
				else {
					token->type = TKN_less;
					ungetc(c, SUBOR);
					break;
				}


			case TKN_gt : // to iste ako TKN_less, ale naopak - neq ...

				if((c = fgetc(SUBOR)) == '=') {		//ak po > nasleduje =
					token->type = TKN_geq;
					break;
				}
				else {				//ak po > nasleduje hocico ine
					token->type = TKN_gt;
					ungetc(c, SUBOR);
					break;
				}

			case TKN_EOL :
				while((c = fgetc(SUBOR)) == '\n') {
					;
				}

				ungetc(c, SUBOR);
				break;


			default :
				break;


		}

		break; //ak prejde cyklus while bez continue, tak skonci

	}

	if(feof(SUBOR)) {			//ak nacitany znak bol EOF, token bude EOF...
		token->type = TKN_EOF;
	}

	add_token(&token_list, token);
	return token;

}
