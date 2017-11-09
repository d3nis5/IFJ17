#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "token.h"

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
bool r_var_declaration();

/* Pravidlo pre <VarDef> 
 * Definicia premennych
 */
bool r_var_definition();

/* Pravidlo pre <Assign> 
 * Priradenie hodnoty do premennej alebo zavolanie funkcie na pravej strane vyrazu
 */
bool r_assign();

/* Pravidlo pre <ExprList> 
 * Zoznam vyrazov pre funkciu print
 */
bool r_expr_list();

/* Pravidlo pre <ItemList> 
 * Zoznam parametrov pri deklarovani/ definicii funcii
 */
bool r_item_list();

/* Pravidlo pre <Item2List> 
 * Pomocne pravidlo pre pravidlo <ItemList>
 */
bool r_item2_list();

/* Pravidlo pre <ParList> 
 * Zoznam parametrov funkcie pri jej volani
 */
bool r_par_list();

/* Pravidlo pre <Par2List>
 * Pomocne pravidlo pre pravidlo <ParList>
 */
bool r_par2_list();

/* Pravidlo pre <RS> 
 * Prava strana priradenia pri pravidle <Assign>
 */
bool r_rhs();

/* Pravidlo pre <Stat> 
 * Statement - prikaz
 */
bool r_stat();

/* Pravidlo pre <StatList> 
 * Statement list - Zoznam prikazov 
 */
bool r_stat_list();

/* Pravidlo pre <Type> 
 * Datove typy
 */
bool r_type();

/* Pravidlo pre <ParPar> 
 * Typy parametrov pri volani funkcie
 */
bool r_par_par();

#endif /* PARSER_H */
