#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SUBOR stdin

typedef enum Druh_tokenu {
	IDENTIF,
	INTGR,
	DBLE,
	RETAZEC,
	PLUS,
	MINUS,
	KRAT,
	LOMITKO,
	S_LOMITKO,
	VACSITKO,
	MENSITKO,
	V_ROVNE,
	M_ROVNE,
	NEROVNOST,
	ROVNASA,
	STREDNIK,
	L_ZATVORKA,
	P_ZATVORKA,
	ZN_EOL,
	ZN_EOF,
	KEYWORD,

} druh_tokenu;

typedef union Hodnota_tokenu {
	char* uk_do_symtab; //ukazatel do tabulky symbolov
	int int_hodnota;	
	double double_hodnota;
	char* retazec; //ukazatel na retazec

	int asci_znaku; //hodnota ascii, znaku, ktory nema predatelnu hodnotu

} hodnota_tokenu;

typedef struct token_t{
	druh_tokenu dt;
	hodnota_tokenu ht;

}token_t;


/* KEYWORDS TODO */

/* preskoci riadkovy komentar po koniec riadku */
void ignor_ria_kom(int *c);

/* preskoci blokovy komentar po ukoncovaciu sekvenciu */
void ignor_blok_kom(int *c);

/* prevedie vsetky pismena parametra na male */
char* to_lower_case(char* string);

/* vrati ukazatel na strukturu token volajucemu */
token_t* get_token();

