/**
 * Názov: gener_buildin_fce.h
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include<stdio.h>
#include<stdlib.h>
#include"instructions.h"

#ifndef BUILDIN_INST
#define BUILDIN_INST

extern instruction_lst list;

/*Vygeneruje instrukcie pre vstavanu funkciu Length. Vrati 0 ked sa to podari, inak COMPILLER_ERR.*/
int generate_length(void);

/*Vygeneruje instrukcie pre vstavanu funkciu SubStr. Vrati 0 ked sa podari, inak COMPILER_ERR.*/
int generate_substr(void);

/*Vygeneruje instrukcie pre vstavanu funkciu Asc. Vrati  ked sa podari,inak COMPILER_ERR*/
int generate_asc(void);

/*Vygeneruje instrukcie pre vstavanu funkciu Chr. Vrati  ked sa podari,inak COMPILER_ERR*/
int generate_chr(void);

/*Zavola vsetky funkcie. Vrati 0 alebo COMPILER_ERR*/
int build_all(void);

#endif /* BUILDIN_INST */
