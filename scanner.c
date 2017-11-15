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
	// -------- TODO --------
}


Ttoken* get_token(Ttoken *token) { 					//nemoze byt void ?????? nie
	
	TKN_type stav; //aktualny stav

/*	Ttoken *token = malloc(sizeof(Ttoken));*/
/*	if(token == NULL) {*/
/*		fprintf(stderr, "Nepodarilo sa naalokovat pamat\n");*/
/*		exit(1);*/
/*	}*/


	int index; //index k roznym ucelom, treba vzdy nulovat

	char *str_pomocny = (char*)malloc(ALOC);			//	TO CHECK - zatial 128.

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
				while(isdigit(c) || isalpha(c) || (c == '_')) {
					str_pomocny[index] = c;
					//token->attribute.st_ptr[index] = c; //ako sa pracuje so symtable ???
					c = fgetc(SUBOR);
					index++;
				}
				str_pomocny[index] = '\0';		//ukoncenie retazca

				token->type = stav; 		 //priradi "stav", teda typ tokenu do tokenu
				token->attribute.string = str_pomocny; //ulozi hodnotu tokenu
				printf("ID je : %s \n", token->attribute.string);
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
					printf("Cele cislo\n");
					break;											// a skonci
				}
				else {	 //TO CHECK ci to ide takto spravit ??
					stav = TKN_dbl;									//znak je bodka -> ide o double
					str_pomocny[index] = c;							//do stringu sa prida bodka
					index++;
				}													//ziaden break -> pokracuje do double.

				
				

			case TKN_dbl :	/* v pripade, ze v TKN_int pride bodka	*/
				// ------------ TODO -----------
				switch(c) {

					case 'E' :
					case 'e' :
						printf("cele cislo s exponentom \n");

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
						printf("Normalny double \n");
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

				printf("Double ma hodnotu: %lf \n", token->attribute.dble);

				break;



			case TKN_str :								//TODO treba doplnit re-alloc ak dojde pamat
				index = 0;

				if(c = fgetc(SUBOR) == '"') {		//ak zacina str spravne ...

					while(c = fgetc(SUBOR) != '"') { //kym neskonci s "
						
						if(c == '\n') { 			//alebo kym nenarazi na koniec riadku
							break;
						} 

						str_pomocny[index] = c;		//zapisuj znaky str, do pomocneho str,
						index++;
					
					}

					str_pomocny[index] = '\0'; //ukonci retazec
					token->type = TKN_str;
					strcpy(token->attribute.string, str_pomocny);
				}

				else {
					fprintf(stderr, "Chybne zadany string format je : !\" \" \n");					
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

			default :
				break;
				

		}

		break; //ak prejde cyklus while bez continue, tak skonci

	}

	if(feof(SUBOR)) {			//ak nacitany znak bol EOF, token bude EOF...
		token->type = TKN_EOF;
	}


	free(str_pomocny);

	return token;
	/* Zvysok TODO */

}

int main(){

	Ttoken *tok = malloc(sizeof(Ttoken));
	if(tok == NULL) {
		fprintf(stderr, "Nepodarilo sa naalokovat pamat\n");
		exit(1);
	}

	tok = get_token(tok);

	/* bude odstranene, je tu len z dovodu nesorsich mojich testov */
}
