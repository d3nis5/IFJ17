#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"


#define SUBOR stdin
#define ALOC 128

/* KEYWORDS TODO */

/* preskoci riadkovy komentar po koniec riadku */
void ignor_ria_kom(int *c);

/* preskoci blokovy komentar po ukoncovaciu sekvenciu */
void ignor_blok_kom(int *c);

/* prevedie vsetky pismena parametra na male */
char* to_lower_case(char* string);

/* vrati ukazatel na strukturu token volajucemu */
Ttoken* get_token();

