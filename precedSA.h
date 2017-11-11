#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

/* TODO zmazat */
#include "parser.h"


#include"error.h"
#include"token.h"

#ifndef PRECEDSA_H
#define PRECEDSA_H

typedef struct tdElem
{
    Ttoken *TokenPtr;				//Ukazatel na token
    struct tdElem *rptr;	//Ukazatel na nasledujuci prvok
	struct tdElem *lptr;	//Ukazatel na predosly prvok

} *tdElemPtr;

/*Zasobnik vo forme obojsmerne viazaneho zoznamu pre precedencnu analyzu*/
typedef struct 
{                        /* dvousměrně vázaný seznam */
	tdElemPtr Bottom;    /* ukazatel na první prvek seznamu */
	tdElemPtr Act;      /* ukazatel na aktuální prvek seznamu */
	tdElemPtr Top;     /* ukazatel na posledni prvek seznamu */
} tdStack;


/*Inicializuje zasobnik, nastavi ukazatele na NULL
**Tato funkcia sa vola len raz na zaciatku prace s vyrazom*/
void tdStackInit(tdStack *);

/*Zrusi cely zasobnik a uvolni pamat poloziek na zasobniku volanim free*/
void tdStackDispose(tdStack *);

/*Vlozi novy prvok na vrchol zasobnika
**v pripade zlyhania alokacie vracia COMPILER_ERR inak 0*/
int tdStackPush(tdStack *, Ttoken *);

/*Nastavi Act na vrchol zasobnika*/
void tdStackActTop (tdStack *);

/*Vrati ukazatel na token z vrcholu zasobnika cez ukazatel na ukazatel na token
** ak je Act neaktivny vrati -1*/
int tdStackTop (tdStack *, Ttoken **);

/*Zrusi prvok na vrchole zasobnika*/
void tdStackDeleteTop (tdStack *);

/*Zrusi prvok pred aktivnym prvkom*/
void tdStackDeletePreAct (tdStack *);

/*Vrati 0 pri uspesnom vlozeni pred aktivny prvok. Ak sa nepodari alokovat pamat vracia COMPILER_ERR*/
int tdStackPreInsert (tdStack *, Ttoken *t);

/*Vrati ukazatel na token aktivneho prvku cez ukazatel na ukazatel na token.
** Ak je zoznam neaktivny vrati -1*/
int tdStackActCopy (tdStack *, Ttoken **);

/*Posunie aktivitu na nasleduji prvok*/
void tdStackSucc (tdStack *);

/*Posunie aktivitu na predosly prvok*/
void tdStackPred (tdStack *);

/*Vracia 0 ak je stack neaktivny*/
int tdStackActive (tdStack *);

/*Vyhodnoti vyraz a vrati nulu v pripade uspechu, inak chybovy kod podla error.h*/
int vyhodnot_vyraz(Ttoken **token);

/*Najde a vrati nablizsi terminal na vrchole zasobniku (ako cislo riadka do precedTab)*/
int getTopTerm(tdStack *);

/*Funckcia co vracia typ terminalu z tokenu*/
TKN_type TokenType(Ttoken *);

/*Na zaklade tokenu vrati index riadka alebo stlpca do precedencnej tabulky **alebo -1 v pripade ze dostane neocakavny token*/
int getIndex(TKN_type);

/*Vrati cislo pravidla ak sa daju zredukovat na zasobniku alebo -1 pri nenajdeni vhodneho pravidla*/
int WhichRule(int *, int);

/*Uplatni dane pravidlo na zasobnik od daneho prvku v zasobniku.
**Vracia 0 ked sa to podari, -1 pri chybe kompilatora*/
int DoRule(tdStack *, int);

#endif 	/* PRECEDSA_H */
