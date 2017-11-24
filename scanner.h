#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"


#define SUBOR stdin
#define ALOC 128

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
void ignor_ria_kom(int *c);

/* preskoci blokovy komentar po ukoncovaciu sekvenciu */
void ignor_blok_kom(int *c);

/* prevedie vsetky pismena parametra na male */
char* to_lower_case(char* string);

/* prevedie string na pozadovany format, pouzitelni instrukciou jazyka IFJ17
** @param string string, ktory treba preformatovat
** @param mem_alloc pamat zatial naalokovana pre string. (v pripade reallocu zvacsena)
*/
char* format_string(char* string, int *mem_alloc);

/* zisti ci je identifikator keyword, ak ano vrati index token type kwd, ak nie vrati 0 cize ID */
int is_kwd(char* ident);

/* vrati ukazatel na strukturu token volajucemu */
Ttoken* get_token(Ttoken *token);

