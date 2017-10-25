#ifndef ERROR_H
#define ERROR_H

/* Definicia navratovych kodov pri chybe */

/* Chyba v ramci lexikalnej analyzy */
#define LEXICAL_ERR 1

/* Chyba v ramci syntaktickej analyzy */
#define SYNTAX_ERR 2

/* Semanticka chyba - nedefinovana funkcia/premenna
 * pokus o redefiniciu funkcie/premennej
 */
#define SEMANT_ERR 3

/* Semanticka chyba typovej kompatibility pri vyrazoch
 * Nespravy pocet alebo typ parametrov pri volaniach funkcii 
 */
#define TYPE_ERR 4

/* Ostatne semanticke chyby */
#define OTHER_SEMANT_ERR 6

/* Vnutorna chyba prekladaca (napr. chyba alokacie pamati,...)*/
#define COMPILER_ERR 99

#endif /* ERROR_H */
