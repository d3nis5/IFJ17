/**
 * Názov: scanner.h
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"

#ifndef SCANNER_H
#define SCANNER_H


#define SUBOR stdin
#define ALOC 128


extern Ttoken_list token_list;


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
Ttoken* get_token();

#endif /* SCANNER_H */
