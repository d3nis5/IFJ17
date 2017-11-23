#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "error.h"

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

extern int err_code;

typedef struct instruction
{
	char *ins;					/* instrukcia aj s operandmi */
	struct instruction *next;	/* ukazatel na dalsiu instrukciu */

} instruction_t;


/* Linearny zoznam v ktorom budu ulozene instrukcie */
typedef struct instruction_list
{
	instruction_t *first;		/* ukazatel na prvu instrukciu v zozname */
	instruction_t *last;		/* ukazatel na poslednu instrukciu v zozname */

} instruction_lst;

/* Inicializuje zoznam instrukcii */
void init_list(instruction_lst *list);

/* Prida instrukciu do zoznamu 'list' 
 * Pri chybe alokacie vracia false a v err_code je chybovy kod, inak vracia true
 */
bool add_instruction(instruction_lst *list, char *inst);

/* Vygeneruje instrukcie pre zaciatok hlavneho tela programu */
bool generate_main(instruction_lst *list);

/* Vypise vsetky instrukcie na stdout */
void print_list(instruction_lst list);

/* Vymaze vsetky polozky zoznamu a aj zoznam */
void delete_list(instruction_lst *list);


#endif /* INSTRUCTIONS_H */
