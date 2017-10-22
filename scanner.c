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

		if((*c) == '\\') { 	//zisti ci je sekvencia ukoncenia kom. dodrzana
			break;		// a opusti preskakovanie komentaru
		}
	}
}


/* prevedie vsetky pismena parametra na male */
char* to_lower_case(char* string) {
	// -------- TODO --------
}


token_t* get_token() {
	
	druh_tokenu stav; //aktualny stav
	hodnota_tokenu hodnota;

	token_t *token = malloc(sizeof(token_t));
	if(token == NULL) {
		fprintf(stderr, "Nepodarilo sa naalokovat pamat\n");
		exit(1);
	}


	int index; //index k roznym ucelom, treba vzdy nulovat

	char *str_pomocny = (char*)malloc(sizeof(char));			//	TO CHECK

	int c = fgetc(SUBOR); //nacitam znaku

	while(!(feof(SUBOR))) { //kym nenarazis na koniec suboru

	
		if((isspace(c)) && (c != '\n')) { //ak je ine prazdne miesto a stav nie je eol
			c = fgetc(SUBOR); //nacitaj dalsi znak	
			continue;		  // a pokracuj vo vyhodnocovani

		}

		if(c == '\n') {			//ak je c znak konca riadku
			token->dt = ZN_EOL;
		}

		if(c == '\'') { //ak je apostrof, ide o riadkovy koment
			ignor_ria_kom(&c);			//pozn: je nacitany \n. Predat EOL token ci nie ??? ***
		}

		if(c == '+') {
			token->dt = PLUS; 	
		}

		if(c == '-') {
			token->dt = MINUS;
		}

		if(c == '*') {
			token->dt = KRAT;
		}

		if(c == '/') {
			stav = LOMITKO;
		}

		if(c == '\\') {			//spatne lomitko ...
			token->dt = S_LOMITKO;
		}

		if(c == '>') {
			stav = VACSITKO;
		}

		if(c == '<') {
			stav = MENSITKO;
		}

		if(c == '=') {
			token->dt = ROVNASA;
		}

		if(c == ';') {
			token->dt = STREDNIK;
		}

		if(c == '(') {
			token->dt = L_ZATVORKA;
		}			

		if(c == ')') {
			token->dt = P_ZATVORKA;
		}	

		if(c == '!') {			//String zacina "!"
			stav = RETAZEC;
		}	
		
		if(isalpha(c) || (c == '_')) {	// _ alebo a-z znaci ID
			stav = IDENTIF;
		}

		if(isdigit(c)) {				//ci je double sa overi az v switchi
			stav = INTGR;
		}


		switch(stav) {
			
			case IDENTIF :
				index = 0;
				while(isdigit(c) || isalpha(c) || (c == '_')) {
					hodnota.uk_do_symtab[index] = c;
					index++;
				}
				token->dt = stav; 		 //priradi "stav", teda typ tokenu do tokenu
				token->ht.uk_do_symtab = hodnota.uk_do_symtab; //ulozi hodnotu tokenu
				break;


			case INTGR :			// -------- TODO --------
				index = 0;
				while(isdigit(c)) {
					
				}

			case DBLE :	/* v pripade, ze v intgr pride bodka	*/	
				// -------- TODO --------


			case RETAZEC :			
				// -------- TODO --------
			

			
			case LOMITKO : //moze byt komentar, alebo deleno
				// -------- TODO --------



			case MENSITKO : //moze ostat mensitkom, alebo pribudne rovnasa
				// -------- TODO --------



			case VACSITKO : // to iste ako mensitko, ale naopak ...	
				// -------- TODO --------		
				break; //len pre prelozitelnost :,D
				

		}

	}

	if(feof(SUBOR)) {			//ak nacitany znak bol EOF, token bude EOF...
		stav = ZN_EOF;
	}

	/* Zvysok TODO */

}

int main(){

	/* bude odstranene, je tu len z dovodu nesorsich mojich testov */
}
