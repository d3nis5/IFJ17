#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include"symtable.h"
#include"error.h"
#include"token.h"

/* TODO zmazat */
#include "gettoken.h"



#ifndef PRECEDSA_H
#define PRECEDSA_H

extern SYMTB_itemptr_g global_symtb;	//ukazatel do globalnej tabulky symbolov(id funkcii)
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

/*Vyhodnoti vyraz a vrati nulu v pripade uspechu, inak chybovy kod podla error.h
*bool znaci ci sa nachadzame v assign(true) vyraze alebo if/dowhile(false) vyraze*/
int vyhodnot_vyraz(Ttoken **token, SYMTB_itemptr_l , bool);

/*Najde a vrati nablizsi terminal na vrchole zasobniku (ako cislo riadka do precedTab)*/
int getTopTerm(tdStack *);

/*Na zaklade tokenu vrati index riadka alebo stlpca do precedencnej tabulky **alebo -1 v pripade ze dostane neocakavny token*/
int getIndex(TKN_type);

/*Vrati cislo pravidla ak sa daju zredukovat na zasobniku alebo -2 pri nenajdeni vhodneho pravidla alebo -3 pri nevhodnom poradi operandov vo vyraze*/
int WhichRule(Ttoken **,SYMTB_itemptr_l, int);

/*Uplatni dane pravidlo na zasobnik od daneho prvku v zasobniku. Premenna typu bool znaci ci sa nachadzame v priradovacom vyraze.
**Vracia 0 ked sa to podari, COMPILER_ERR pri chybe kompilatora alebo TYPE_ERR pri typovej chybe vo vyraze alebo OTHER_SEMANT_ERR pri semantickej chybe*/
int DoRule(tdStack *,SYMTB_itemptr_l, int, bool);

TKN_type TokenType(Ttoken *t);


#endif /* PRECEDSA_H */

