/**
 * Názov: parser.h
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "precedsa.h"
#include "symtable.h"
#include "error.h"
#include "instructions.h"
#include "gener_buildin_fce.h"
#include "scanner.h"


#ifndef PARSER_H
#define PARSER_H

Ttoken_list token_list;
instruction_lst list;					/* paska instrukcii */
Ttoken *token;

/* Pravidlo pre <Program> */
bool r_program();

/* Pravidlo pre <FceDec> 
 * Deklaracia a definicia funkcii
 */
bool r_fc_dec();

/* Pravidlo pre <Declaration>
 * Deklaracia funkcii
 */
bool r_declaration();

/* Pravidlo pre <Definition> 
 * Definicia funkcii
 */
bool r_definition();

/* Pravidlo pre <VarDec> 
 * Deklaracia a definicia premennych
 */
bool r_var_declaration(SYMTB_itemptr_l*);

/* Pravidlo pre <VarDef> 
 * Definicia premennych
 * var - premenna do ktorej sa bude priradovat
 */
bool r_var_definition(SYMTB_itemptr_l*, SYMTB_itemptr_l var);

/* Pravidlo pre <Assign> 
 * Priradenie hodnoty do premennej alebo zavolanie funkcie na pravej strane vyrazu
 */
bool r_assign(SYMTB_itemptr_l);

/* Pravidlo pre <ExprList> 
 * Zoznam vyrazov pre funkciu print
 */
bool r_expr_list(SYMTB_itemptr_l);

/* Pravidlo pre <ItemList> 
 * Zoznam parametrov pri deklarovani/ definicii funcii
 */
bool r_item_list(SYMTB_itemptr_g, bool definition);

/* Pravidlo pre <Item2List> 
 * Pomocne pravidlo pre pravidlo <ItemList>
 */
bool r_item2_list(SYMTB_itemptr_g, bool definition);

/* Pravidlo pre <ParList> 
 * Zoznam parametrov funkcie pri jej volani
 */
bool r_par_list(SYMTB_itemptr_g, SYMTB_itemptr_l);

/* Pravidlo pre <Par2List>
 * Pomocne pravidlo pre pravidlo <ParList>
 */
bool r_par2_list(SYMTB_itemptr_g, SYMTB_itemptr_l);

/* Pravidlo pre <RS> 
 * Prava strana priradenia pri pravidle <Assign>
 * type - ocakavany typ
 */
bool r_rhs(SYMTB_itemptr_l, char type);

/* Pravidlo pre <Stat> 
 * Statement - prikaz
 */
bool r_stat(SYMTB_itemptr_l*);

/* Pravidlo pre <StatList> 
 * Statement list - Zoznam prikazov 
 */
bool r_stat_list(SYMTB_itemptr_l*);

/* Pravidlo pre <Type> 
 * Datove typy
 */
bool r_type();

/* Pravidlo pre <ParPar> 
 * Typy parametrov pri volani funkcie
 */
bool r_par_par(SYMTB_itemptr_g, SYMTB_itemptr_l);

/* Vrati datovy typ tokenu ako 1 znak {'i', 's', 'd'} */
char type2char(Ttoken *token);

/* Skontroluje ci vsetky deklarovane funkcie boli definovane */
bool check_functions(SYMTB_itemptr_g);

/* Preskoci tokeny noveho riadku TKN_EOL 
 * V pripade lexikalnej chyby vracia false, inak vracia true
 */
bool skip_EOL();

#endif /* PARSER_H */
