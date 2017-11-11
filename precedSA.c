#include "precedSA.h"

#define MAXPOLE 10

/*Tabulka precedencnych pravidiel*/
char PrecTab[18][18]={
	">>>>>>>>>>><><<<<>", // * 0
	"<>><<>>>>>><><<<<>", // + 1
	"<>><<>>>>>><><<<<>", // - 2
	">>>>>>>>>>><><<<<>", // / 3
	"<>><>>>>>>><><<<<>", // \ */ 4
	"<<<<<>>>>>><><<<<>", // < 5
	"<<<<<>>>>>><><<<<>", // > 6
	"<<<<<>>>>>><><<<<>", // = 7
	"<<<<<>>>>>><><<<<>", // <= 8
	"<<<<<>>>>>><><<<<>", // >= 9
	"<<<<<>>>>>><><<<<>", // <> 10
	"<<<<<<<<<<<<=<<<<E", // ( 11
	">>>>>>>>>>>E>EEEE>", // ) 12
	">>>>>>>>>>>E>EEEE>", // id 13
	">>>>>>>>>>>E>EEEE>", // int 14
	">>>>>>>>>>>E>EEEE>", // double 15
	">>>>>>>>>>>E>EEEE>", // str 16
	"<<<<<<<<<<<<E<<<<$"  //$ EOL ; 17
	};

int vyhodnot_vyraz(Ttoken **token)
{
	int chyba;
	char pravidlo;
	int riadok,stlpec;
	Ttoken *t, *t2, ts, th;	//t2 je pomocny pointer na token a ts a th je pomocny token
	tdStack S;		//zasobnik

	t=*token; //prvy token

	tdStackInit(&S);
	th.type='<';	//pomocny token ktory obsahuje zaciatok handle
	ts.type=TKN_EOL;	//pomocny token
	chyba=tdStackPush(&S, &ts);	//vlozi EOL($) na bottom zasobnika
	if (chyba)
	{
		tdStackDispose(&S);
		return COMPILER_ERR;
	}
	do
	{
		riadok=getTopTerm(&S);
        stlpec=getIndex(TokenType(t));
		if(stlpec < 0)	//nasiel token ktory necakal
		{
			*token=t;	//vratim ukazatel na token ktory nevyhovuje
			tdStackDispose(&S);
			return SYNTAX_ERR;
		}
		pravidlo=PrecTab[riadok][stlpec];
		switch (pravidlo)
		{
			case '=':	chyba=tdStackPush(&S, t);	//push token stlpca a precita dalsi symbol zo vstupu
						if (chyba)
					    {
        					tdStackDispose(&S);
        					return COMPILER_ERR;
    					}
						t=get_token();	//TODO
						break;

			case '<':	tdStackActTop(&S);	//nastavi aktivitu na vrchol zasobnika
						tdStackTop(&S, &t2);
						if ( getIndex(TokenType(t2)) == 19)
						{
                            chyba=tdStackPreInsert(&S, &th);	//push '<' pred vrchol zasobnika*/
							if (chyba)
                        	{
                            	tdStackDispose(&S);
                            	return COMPILER_ERR;
                        	}
						}
                        else
						chyba=tdStackPush(&S, &th);
						if (chyba)
                        {
                            tdStackDispose(&S);
                            return COMPILER_ERR;
                        }

						chyba=tdStackPush(&S, t);    //push token stlpca a precita dalsi symbol zo vstupu
						if (chyba)
                        {
                            tdStackDispose(&S);
                            return COMPILER_ERR;
                        }

                        t=get_token();	//TODO
                        break;

			case '>':	{
							int pole[MAXPOLE];
							int poc, znak, rule;

							tdStackActTop(&S);   //nastavi sa na vrchol zasobnika
					    	tdStackTop(&S, &t2);  //precita vrchol zasobnika
							znak=getIndex(TokenType(t2));
							poc=1;	//precitali sme 1 ukazatel na token
							while (znak != 18 && znak != 17)	//citame vsetko od vrcholu po prvy vyskyt '<' alebo EOL($)
    						{
        						tdStackPred(&S);
        						tdStackActCopy(&S, &t2);
								znak=getIndex(TokenType(t2));
								poc++;	//pocitam si kolko znakov je od vrcholu po '<'
    						}
							for (int i=0; i<poc; i++)
							{
								if (i>=MAXPOLE)
									printf("maxpole error\n"); //TODO ERROR

                                pole[i]=getIndex(TokenType(t2));
								tdStackSucc(&S);	//preskocime '<'
								tdStackActCopy(&S, &t2);
							}
							rule=WhichRule(pole, poc);
							if (rule < 0)	//ziadne pravidlo nevyhovuje
							{
								tdStackDispose(&S);
                                return SYNTAX_ERR;
							}
							int v=DoRule(&S, rule);
							if (v<0)
							{
								tdStackDispose(&S);
								return COMPILER_ERR;
							}
						}
						break;

			case 'E':	tdStackDispose(&S);
                        return SYNTAX_ERR;
			default: break;
		}
	}while( stlpec != 17 || riadok != 17 );

	*token=t;	//vratim ukazatel na dalsi token
	tdStackDispose(&S);
	return 0;
}

int DoRule(tdStack *S, int rule)
{
	int chyba;
	Ttoken *new;
	if (rule == 1)	// '*'
	{
		//TODO semantic
		for(int i=0; i<4; i++)	//vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
		{
			tdStackDeleteTop(S);
		}
		new=malloc(sizeof(struct token));
		if (new == NULL)
		{
			return -1;
		}
		new->type='N';
		chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
	}
	else if (rule == 2)	// '+'
	{
		//TODO semantic
		for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
	}
    else if (rule == 3)	// '-'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
		{
            return -1;
		}
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 4)	// '/'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 5) // '\'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 6)	// '<'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
			return -1;
    }
    else if (rule == 7)	// '>'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 8)	// '='
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 9)	// '<='
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 10)	// '>='
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 11)	// '<>'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 12)	// '(N)'
    {
        //TODO semantic
        for(int i=0; i<4; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
	else if (rule == 13)
	{
		//TODO semantic
        for(int i=0; i<2; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }

		new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
       	chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
	}
    else if (rule == 14)
    {
        //TODO semantic
        for(int i=0; i<2; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }

        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
	else if (rule == 15)
    {
        //TODO semantic
        for(int i=0; i<2; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }

        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
    else if (rule == 16)
    {
        //TODO semantic
        for(int i=0; i<2; i++)  //vymaze pocet prvkov na zasobniku kolko sa pomocou pravidla zredukuje
        {
            tdStackDeleteTop(S);
        }
        new=malloc(sizeof(struct token));
        if (new == NULL)
        {
            return -1;
        }
        new->type='N';
        chyba=tdStackPush(S, new);
		if (chyba)
            return -1;
    }
	else
		return 1; //TODO ??
	return 0;
}

int WhichRule(int *pole, int max)
{
    if (pole[0] == '<')
        return -1;
	if (max == 4)
	{
		if ((pole[1] == 19 && pole[3] == 19) ||	(pole[1] == 11 && pole[3] == 12) )//'N'
		{
			switch (pole[2])
			{
				case 0: return 1;	// '*'
				case 1: return 2;	// '+'
				case 2: return 3;	// '-'
				case 3: return 4;	// '/'
				case 4: return 5;	// '\'
				case 5: return 6;	// '<'
				case 6: return 7;	// '>'
				case 7: return 8;	// '='
				case 8: return 9;	// '<='
				case 9: return 10;	// '>='
				case 10: return 11;	// '<>'
				case 19: return 12;	// 'N'
				default: return -1;	//Ziadne pravidlo
			}
		}
	}
	else if (max == 2)
	{
		if (pole[1] == 13)	//id
			return 13;
		else if (pole[1] == 14)	//int
			return 14;
		else if (pole[1] == 15)	//double
			return 15;
		else if (pole[1] == 16)	//string
			return 16;
		else				//Ziadne pravidlo nevyhovuje
			return -1;
	}
	else
		return -1;	//Ziadne pravidlo nevyhovuje
}

TKN_type IndextoTKN_type(int v)
{
	if (v == 0)
        return TKN_star;
    if (v == 1)
        return TKN_plus;
    if (v == 2)
        return TKN_minus;
    if (v == 3)
        return TKN_slash;
    if (v == 4)
        return TKN_backslash;
    if (v == 5)
        return TKN_less;
    if (v == 6)
        return TKN_gt;
    if (v == 7)
        return TKN_neq;
    if (v == 8)
        return TKN_leq;
    if (v == 9)
        return TKN_geq;
    if (v == 10)
        return TKN_neq;
    if (v == 11)
        return TKN_leftpar;
    if (v == 12)
        return TKN_rightpar;
    if (v == 13)
        return TKN_id;
    if (v == 14)
        return TKN_int;
    if (v == 15)
        return TKN_dbl;
    if (v == 16)
        return TKN_str;
    if (v == 17)
        return TKN_EOL;	// || TKN_smcolon
	if (v == 18)
		return '<';
	if (v == 19)
		return 'N';
}

int getIndex(TKN_type type)
{
	if (type == TKN_star)
		return 0;
	if (type == TKN_plus)
		return 1;
	if (type == TKN_minus)
        return 2;
	if (type == TKN_slash)
        return 3;
	if (type == TKN_backslash)
        return 4;
	if (type == TKN_less)
        return 5;
	if (type == TKN_gt)
        return 6;
	if (type == TKN_eq)
        return 7;
	if (type == TKN_leq)
        return 8;
	if (type == TKN_geq)
        return 9;
	if (type == TKN_neq)
        return 10;
	if (type == TKN_leftpar)
        return 11;
	if (type == TKN_rightpar)
        return 12;
	if (type == TKN_id)
        return 13;
	if (type == TKN_int)
        return 14;
	if (type == TKN_dbl)
        return 15;
	if (type == TKN_str)
        return 16;
	if (type == TKN_EOL || type == TKN_smcolon || type == KWD_then)	// $
        return 17;
	if (type == '<')	//zaciatok handle
		return 18;
	if (type == 'N')	//neterminal
		return 19;
	return -1;	//nasiel neocakavany token
}

TKN_type TokenType(Ttoken *t)
{
	return (t->type);
}

int getTopTerm(tdStack *S)
{
	int riadok;		//index riadka do precedencnej tabulky
	Ttoken *tmp;

	tdStackActTop(S);   //nastavi sa na vrchol zasobnika a precita ho
    tdStackActCopy(S, &tmp);
	do
	{
		riadok=getIndex( TokenType(tmp) );	//zisti co za typ tokenu je na vrchole a priradi mu index
		if (riadok >= 0 && riadok <=17)		//je terminal
			break;

		tdStackPred(S);
		tdStackActCopy(S, &tmp);
	} while (riadok != 17);	//na spodku zasobnika je EOL($)

	return riadok;
}


void tdStackInit(tdStack *S)
{
	S->Bottom=NULL;
	S->Act=NULL;
	S->Top=NULL;
	return;
}

void tdStackDispose(tdStack *S)
{
	tdElemPtr tmp;	//pomocna premenna
	S->Act=S->Bottom;	//nastavim sa na zaciatok zoznamu
	while(S->Act != NULL){
		tmp=S->Act;
		S->Act=S->Act->rptr;	//posun na dalsi prvok
		if (tmp->TokenPtr->type == 'N')
            free(tmp->TokenPtr);
		free(tmp);
	}
	S->Act=NULL;
	S->Bottom=NULL;
	S->Top=NULL;
	return;
}

int tdStackPush(tdStack *S, Ttoken *t)
{
	tdElemPtr new;	//novy prvok
	if ((new=malloc(sizeof(struct tdElem))) == NULL){	//chyba pri alokovani
		return COMPILER_ERR;
	}
	new->TokenPtr=t;
	new->rptr=NULL;	//new bude poslednym prvkom
	new->lptr=S->Top;
	if (S->Top != NULL)
		S->Top->rptr=new;
	else
		S->Bottom=new;	//zoznam bol prazdny
	S->Top=new;
	return 0;
}

void tdStackActTop(tdStack *S)
{
	S->Act=S->Top;
	return;
}

int tdStackTop(tdStack *S, Ttoken **t)
{
	if (S->Top == NULL)
		return -1;
	else
		*t=S->Top->TokenPtr;
	return 0;
}

void tdStackDeleteTop (tdStack *S)
{
	tdElemPtr tmp;	//pomocna premenna
	if (S->Top != NULL){
		tmp=S->Top;
		if (S->Act == S->Top)	//posledny prvok bol aktivny
			S->Act=NULL;
		if (S->Top == S->Bottom){	//zoznam ma len 1 prvok
			S->Top=NULL;
			S->Bottom=NULL;
		}
		else{	//v zozname je viac ako 1 prvok
			S->Top=S->Top->lptr;
			S->Top->rptr=NULL;
		}
		if (tmp->TokenPtr->type == 'N')
        {
          free(tmp->TokenPtr);
        }
		free(tmp);
	}
	return;
}

void tdStackDeletePreAct (tdStack *S)
{
	if (S->Act != NULL && S->Act != S->Bottom){
		if (S->Act->lptr != NULL){	//zistim ci je co rusit
			tdElemPtr tmp;
			tmp=S->Act->lptr;
			S->Act->lptr=tmp->lptr;	//preklenutie ruseneho
			if (tmp == S->Bottom)	//ruseny prvok je prvy
				S->Bottom=S->Act;
			else
				tmp->lptr->rptr=S->Act;
            if (tmp->TokenPtr->type == 'N')
                free(tmp->TokenPtr);
			free(tmp);
		}
	}
	return;
}

int tdStackPreInsert (tdStack *S, Ttoken *t)
{
	if (S->Act != NULL)
	{
		tdElemPtr new;
		if ((new=malloc(sizeof(struct tdElem))) == NULL)
		{  //chyba pri alokacii
            return COMPILER_ERR;
		}
		new->TokenPtr=t;
		new->lptr=S->Act->lptr;
		new->rptr=S->Act;
		S->Act->lptr=new;
		if (S->Act == S->Bottom)	//aktualny prvok je prvy
			S->Bottom=new;
		else
			new->lptr->rptr=new;
	}
	return 0;
}

int tdStackActCopy (tdStack *S, Ttoken **t)
{
	if (S->Act == NULL)
		return -1;		//Chyba pokusame sa pracovat s NULL ukazatelom
	else
		*t=S->Act->TokenPtr;
	return 0;
}

void tdStackSucc (tdStack *S)
{
	if (S->Act != NULL)
		S->Act=S->Act->rptr;
	return;
}

void tdStackPred (tdStack *S)
{
	if (S->Act != NULL)
		S->Act=S->Act->lptr;
	return;
}

int tdStackActive (tdStack *S)
{
	return (S->Act != NULL);
}

