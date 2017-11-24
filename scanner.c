#include "scanner.h"


/* preskoci riadkovy komentar po koniec riadku */
void ignor_ria_kom(int *c) { 
	while((*c) != '\n') {
		(*c) = fgetc(SUBOR);
	}
}

/* preskoci blokovy komentar po ukoncovaciu sekvenciu */
void ignor_blok_kom(int *c) {

	while(!(feof(SUBOR))) {	
		while((*c) != '\'') { //kym znak nie je apostrof
			(*c) = fgetc(SUBOR);
		}
		(*c) = fgetc(SUBOR); //skonci cyklus, teda natrafi na " ' ", nacita novy znak

		if((*c) == '/') { 	//zisti ci je sekvencia ukoncenia kom. dodrzana
			break;		// a opusti preskakovanie komentaru
		}
	}
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
	int pamat = *mem_alloc;

	int i = 0;

	//printf("Mem vo format: %d \n", pamat);	
	while(string[i]) {


		if((pamat - 1) < (i + 5)) {	
 
			pamat = pamat*2;							//zdvojnasob mem_allocated
			//string = realloc(string, pamat);					//reallocuj str_pomocny TODO
			//printf("(format)Bola zvacsena alloc. pamat na %d\n", pamat);

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


/* zisti ci je identifikator keyword, ak ano vrati index token type kwd, ak nie vrati 0 cize ID */
int is_kwd(char* ident) {
	for(int i = 0; i < 35; i++) {
		if((strcmp(kwds[i], ident) == 0)) {
			return (i + 21);				//vrati index do TKN_type
		}
	}
	return 0;								//ostane ID
}


Ttoken* get_token(Ttoken *token) { 	
	
	TKN_type stav; //aktualny stav

/*	Ttoken *token = malloc(sizeof(Ttoken));*/
/*	if(token == NULL) {*/
/*		fprintf(stderr, "Nepodarilo sa naalokovat pamat\n");*/
/*		exit(1);*/
/*	}*/



	int index; //index k roznym ucelom, treba vzdy nulovat

	int mem_allocated;

	char *str_pomocny = malloc(ALOC);			//	TO CHECK - zatial 128. //odstranil som (char*)

	int c = fgetc(SUBOR); //nacitam znaku

	while(!(feof(SUBOR))) { //kym nenarazis na koniec suboru

	
		if((isspace(c)) && (c != '\n')) { //ak je ine prazdne miesto a stav nie je eol
			c = fgetc(SUBOR); //nacitaj dalsi znak	
			continue;		  // a pokracuj vo vyhodnocovani

		}

		if(c == '\n') {			//ak je c znak konca riadku
			token->type = TKN_EOL;
		}

		if(c == '\'') { //ak je apostrof, ide o riadkovy koment
			ignor_ria_kom(&c);			//pozn: je nacitany \n. Predat EOL token ci nie ??? ***
		}

		if(c == '+') {
			token->type = TKN_plus; 	
		}

		if(c == '-') {
			token->type = TKN_minus;
		}

		if(c == '*') {
			token->type = TKN_star;
		}

		if(c == '/') {
			stav = TKN_slash;
		}

		if(c == '\\') {			//spatne TKN_slash ...
			token->type = TKN_backslash;
		}

		if(c == '>') {
			stav = TKN_gt;
		}

		if(c == '<') {
			stav = TKN_less;
		}

		if(c == '=') {
			token->type = TKN_eq;
		}

		if(c == ';') {
			token->type = TKN_smcolon;
		}

		if(c == '(') {
			token->type = TKN_leftpar;
		}			

		if(c == ')') {
			token->type = TKN_rightpar;
		}	

		if(c == '!') {			//String zacina " !" "
			stav = TKN_str;
		}	
		
		if(isalpha(c) || (c == '_')) {	// _ alebo a-z znaci ID
			stav = TKN_id;
		}

		if(isdigit(c)) {				//ci je double sa overi az v switchi
			stav = TKN_int;
		}


		switch(stav) {
			

			case TKN_id :
				index = 0;
				mem_allocated = ALOC;				//alokovana pamat pre pomocny string
				while(isdigit(c) || isalpha(c) || (c == '_')) {

					if((mem_allocated - 1) == index) {
						mem_allocated *= 2;									//zdvojnasob mem_allocated
						str_pomocny = realloc(str_pomocny, mem_allocated);	//reallocuj str_pomocny
						//printf("Bola zvacsena alloc. pamat na %d\n", mem_allocated);
					}

					str_pomocny[index] = c;
					c = fgetc(SUBOR);
					index++;
				}
				str_pomocny[index] = '\0';		//ukoncenie retazca

				str_pomocny = to_lower_case(str_pomocny);
				token->type = is_kwd(str_pomocny); 		 //priradi "stav", teda typ tokenu do tokenu

				token->attribute.string = str_pomocny;

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

						c = fgetc(SUBOR);
						if(isdigit(c)) {
							str_pomocny[index] = c;
							index++;
						}
						else {
							fprintf(stderr, "Lex Err: expont musi obsahovat nepr. postupnost cisl.\n");
							exit(1);
						}

						c = fgetc(SUBOR);
						while(isdigit(c)) {
							str_pomocny[index] = c;
							index++;
							c = fgetc(SUBOR);
						}

						break;

					case '.' :
						//printf("Normalny double \n");
						c = fgetc(SUBOR); 				//nacitaj prvy znak po bodke

						if(!(isdigit(c))) {
							fprintf(stderr, "Lexikalna chyba double ma tvar num.num\n");
							exit(1);
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
								exit(1);
							}
							
							c = fgetc(SUBOR);

							while(isdigit(c)) {
								str_pomocny[index] = c;
								index++;
								c = fgetc(SUBOR);
							}
							
						}

						break;
				}
				token->type = TKN_dbl;
				token->attribute.dble = strtod(str_pomocny, NULL);

				//printf("Double ma hodnotu: %lf \n", token->attribute.dble);

				break;



			case TKN_str :	
				index = 0;
				mem_allocated = ALOC;				//alokovana pamat pre pomocny string

				if((c = fgetc(SUBOR)) == '"') {		//ak zacina str spravne ...

					c = fgetc(SUBOR);

					while((c != '"')) { //kym neskonci s "
						
						if(c == '\n') { 			//alebo kym nenarazi na koniec riadku
							break;
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
					str_pomocny = to_lower_case(str_pomocny);
					str_pomocny = format_string(str_pomocny, &mem_allocated);
					token->attribute.string = str_pomocny;

				}

				else {
					fprintf(stderr, "Chybne zadany string format je : !\" \" \n");
					exit(1);					
				}
				break;

						
			case TKN_slash : //moze byt komentar, alebo deleno

				if(c = fgetc(SUBOR) == '\'') {
					ignor_blok_kom(&c);
					continue;			//
				}

				else { //je to lomitko ...
					token->type = TKN_slash;
				}




			case TKN_less : //moze ostat TKN_lessm, alebo pribudne TKN_eq
				
				if(c = fgetc(SUBOR) == '>') { 		//ak za < nasleduje >
					token->type = TKN_neq;
					break;
				}
				else if(c == '=') { 				//ak za < nasleduje = -> '<='
					token->type = TKN_leq;
					break;
				}
				else {
					token->type = TKN_less;
					break;
				}


			case TKN_gt : // to iste ako TKN_less, ale naopak - neq ...	

				if(c = fgetc(SUBOR) == '=') {		//ak po > nasleduje =
					token->type = TKN_geq;
					break;
				}
				else {				//ak po > nasleduje hocico ine
					token->type = TKN_gt;
					break;
				}

			case TKN_EOL :

				while(c = fgetc(SUBOR) == '\n') {
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


	return token;

}
