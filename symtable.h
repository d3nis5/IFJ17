#include <stdbool.h>
#include <string.h>

#ifndef SYMTABLE_H
#define SYMTABLE_H

#define MAX_PAR 128		/* maximalny pocet parametrov funkcie */

/* Tabulka symbolov implementovana v podobe binarneho vyhladavacieho stromu 
 * SYMTB_itemptr_g		polozka globalnej tabulky symbolov, su v nej iba funkcie
 * SYMTB_itemptr_l 		polozka lokalnej tabulky symbolov, su v nej iba premenne
 * DT_type				enum datovych typov
 */ 

typedef enum data_type {
	TYPE_integer,			/* datovy typ INTEGER */
	TYPE_double,			/* datovy typ DOUBLE */
	TYPE_string,			/* datovy typ STRING */
} DT_type;		


typedef struct local_symtable_item
{
	char *var_name;						/* nazov premennej */
	bool var_declared;					/* bola premenna deklarovana */
	bool var_defined;					/* bola premenna definovana */
	DT_type type;						/* datovy typ premennej */
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
	DT_type ret_type;					/* navratovy typ funkcie */
	DT_type parameters[MAX_PAR];		/* parametre funkcie */
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
 * !!!!!!!!!!!Pri uspechu vracia tiez NULL ale err_code je nezmenene!!!!!!!!!!
 * Ak sa v tabulke uz nachadza premenna s tymto menom vracia sa ukazatel na nu
 */
SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, bool declared, bool defined, DT_type type);

/* Zrusi celu lokalnu tabulku symbolov */
void LST_delete_tab(SYMTB_itemptr_l *);


/* ----------FUNKCIE PRE PRACU S GLOBALNOU TABULKOU SYMBOLOV---------- */

/* Inicializacia globalnej tabulky symbolov */
void GST_init(SYMTB_itemptr_g *);

/* Vyhladanie funkcie s menom 'name' v globalnej tabulke symbolov 
 * Ak sa v tabulke tato premenna nenachadza, vracia sa NULL
 */
SYMTB_itemptr_g GST_search(SYMTB_itemptr_g RootPtr, char *name);

/* Prida funkciu do tabulky symbolov 
 * Pri chybe alokacie vracia NULL a v err_code je kod chyby
 * !!!!!!!!!!!Pri uspechu vracia tiez NULL ale err_code je nezmenene!!!!!!!!!!
 * Ak sa v tabulke uz nachadza funkcia s tymto menom vracia sa ukazatel na nu
 */
SYMTB_itemptr_g GST_add_function(SYMTB_itemptr_g *RootPtr, char *name, bool declared, bool defined, DT_type type);

/* Zrusi celu globalnu tabulku symbolov */
void GST_delete_tab(SYMTB_itemptr_g *);


#endif /* SYMTABLE_H */
