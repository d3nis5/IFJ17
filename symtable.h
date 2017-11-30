/**
 * Názov: symtable.h
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#ifndef SYMTABLE_H
#define SYMTABLE_H

#define MAX_PAR 128		/* maximalny pocet parametrov funkcie */

char *builtin1, *builtin2, *builtin3, *builtin4;

/* Tabulka symbolov implementovana v podobe binarneho vyhladavacieho stromu 
 * SYMTB_itemptr_g		polozka globalnej tabulky symbolov, su v nej iba funkcie
 * SYMTB_itemptr_l 		polozka lokalnej tabulky symbolov, su v nej iba premenne
 */ 


extern int err_code;

typedef struct local_symtable_item
{
	char *var_name;						/* nazov premennej */
	char type;							/* datovy typ premennej {'i', 'd', 's'} */
	union								/* hodnota premennej */
	{
		int int_value;
		double dbl_value;
		char *str_value;
	} value;
	struct local_symtable_item *lptr;	/* ukazatel na lavy podstrom */
	struct local_symtable_item *rptr;	/* ukazatel na pravy podstrom */

} *SYMTB_itemptr_l;		/* ukazatel na polozku v lokalnej tabulke symbolov(premenna) */


typedef struct global_symtable_item
{
	char *function_name;				/* nazov funkcie */
	bool fc_declared;					/* bola funkcia deklarovana? */
	bool fc_defined;					/* bola funkcia definovana? */ 
	char ret_type;						/* datovy typ premennej {'i', 'd', 's'} */
	int par_count;						/* pocet parametrov funkcie*/
	char *par_names[MAX_PAR];			/* identifikatory parametrov */
	char parameters[MAX_PAR];			/* parametre funkcie, retazec ukonceny 0 */
	SYMTB_itemptr_l local_symtb;		/* ukazatel do lokalnej tabulky symbolov */
	struct global_symtable_item *lptr;	/* ukazatel na lavy podstrom */
	struct global_symtable_item *rptr;	/* ukazatel na pravy podstrom */
		
} *SYMTB_itemptr_g;		/* ukazatel na polozku v globalnej tabulke symbolov(funkcia) */




/* ----------FUNKCIE PRE PRACU S LOKALNOU TABULKOU SYMBOLOV---------- */

/* Inicializacia lokalnej tabulky symbolov */
void LST_init(SYMTB_itemptr_l *);

/* Vyhladanie premennej s menom 'name' v lokalnej tabulke symbolov 
 * Ak sa v tabulke tato premenna nenachadza, vracia sa NULL
 */
SYMTB_itemptr_l LST_search(SYMTB_itemptr_l RootPtr, char *name);

/* Prida premennu do tabulky symbolov 
 * Pri chybe alokacie vracia NULL a v err_code je kod chyby
 * Pri uspechu vracia ukazatel na novu polozku
 * Ak sa v tabulke uz nachadza premenna s tymto menom vracia sa ukazatel na nu a err_code = -1
 */
SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, char type);

/* Zrusi celu lokalnu tabulku symbolov */
void LST_delete_tab(SYMTB_itemptr_l *);


/* ----------FUNKCIE PRE PRACU S GLOBALNOU TABULKOU SYMBOLOV---------- */

/* TODO zmazat */
void Print_tree_g(SYMTB_itemptr_g TempTree);
void Print_tree_l(SYMTB_itemptr_l TempTree);


/* Inicializacia globalnej tabulky symbolov */
void GST_init(SYMTB_itemptr_g *);

/* Vyhladanie funkcie s menom 'name' v globalnej tabulke symbolov 
 * Ak sa v tabulke tato premenna nenachadza, vracia sa NULL
 */
SYMTB_itemptr_g GST_search(SYMTB_itemptr_g RootPtr, char *name);

/* Prida funkciu do tabulky symbolov 
 * Pri chybe alokacie vracia NULL a v err_code je kod chyby
 * Pri uspechu vracia ukazatel na novu polozku
 * Ak sa v tabulke uz nachadza funkcia s tymto menom vracia sa ukazatel na nu a err_code = -1
 */
SYMTB_itemptr_g GST_add_function(SYMTB_itemptr_g *RootPtr, char *name, bool declared, bool defined, char type, char *params);

/* Prida parameter typu 'type' funkcii na ktoru ukazuje ukazatel 'function'
 * Vracia false ak sa nepodari pridat parameter z dovodu prekrocenia pola,
 * inak vracia true
 */
bool GST_add_par(SYMTB_itemptr_g function, char type);

/* Do tabulky symbolov vlozi vsetky vstavane funkcie 
 * Pri neuspechu vracia false, inak true
 */
bool GST_add_builtin(SYMTB_itemptr_g *table);

/* Zrusi celu globalnu tabulku symbolov a aj kazdu lokalnu tabulku */
void GST_delete_tab(SYMTB_itemptr_g *);

void GST_free_builtin_names();


#endif /* SYMTABLE_H */
