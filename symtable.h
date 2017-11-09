#include <stdbool.h>
#include <string.h>

#ifndef SYMTABLE_H
#define SYMTABLE_H


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


typedef struct global_symtable_item
{
	char *function_name;			/* nazov funkcie */
	bool fc_declared;				/* bola funkcia deklarovana? */
	bool fc_defined;				/* bola funkcia definovana? */ 
	DT_type ret_type;				/* navratovy typ funkcie */
	SYMTB_itemptr_l local_symtb;	/* ukazatel do lokalnej tabulky symbolov */
	global_symtable_item *lptr;		/* ukazatel na lavy podstrom */
	global_symtable_item *rptr;		/* ukazatel na pravy podstrom */
		
	/* TODO */
	/* funkcia: 	-parametre

	   premenna: 	-hodnota
					
	   ukazatel na pravy a lavy podstrom
	*/


} *SYMTB_itemptr_g;		/* ukazatel na polozku v globalnej tabulke symbolov(funkcia) */


typedef struct local_symtable_item
{
	char *var_name;					/* nazov premennej */
	bool var_declared;				/* bola premenna deklarovana */
	bool var_defined;				/* bola premenna definovana */
	DT_type type;					/* datovy typ premennej */
	local_symtable_item *lpt;		/* ukazatel na lavy podstrom */
	local_symtable_item *rptr;		/* ukazatel na pravy podstrom */

} *SYMTB_itemptr_l;		/* ukazatel na polozku v lokalnej tabulke symbolov(premenna) */



/* ----------FUNKCIE PRE PRACU S LOKALNOU TABULKOU SYMBOLOV---------- */

/* Inicializacia lokalnej tabulky symbolov */
void LST_init(SYMTB_itemptr_l *);

/* Vyhladanie premennej s menom 'name' v lokalnej tabulke symbolov 
 * Ak sa v tabulke tato premenna nenachadza, vracia sa NULL
 */
SYMTB_itemptr_l LST_search(SYMTB_itemptr_l RootPtr, char *name);

/* Prida premennu do tabulky symbolov */
SYMTB_itemptr_l LST_add_var(SYMTB_itemptr_l *RootPtr, char *name, bool declared, bool defined, DT_type type);



/* ----------FUNKCIE PRE PRACU S GLOBALNOU TABULKOU SYMBOLOV---------- */

/* Inicializacia globalnej tabulky symbolov */
void GST_init(SYMTB_itemptr_g *);

/* Vyhladanie funkcie s menom 'name' v globalnej tabulke symbolov 
 * Ak sa v tabulke tato premenna nenachadza, vracia sa NULL
 */
SYMTB_itemptr_g GST_search(SYMTB_itemptr_g RootPtr, char *name);

/* Prida funkciu do tabulky symbolov */
SYMTB_itemptr_l GST_add_function(SYMTB_itemptr_g *RootPtr, char *name, bool declared, bool defined, DT_type type);


#endif /* SYMTABLE_H */
