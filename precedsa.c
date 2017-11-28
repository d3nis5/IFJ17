/**
 * Názov: precedSA.c
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include "precedsa.h"
#define MAXPOLE 10
#define MAX_RET 150

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

int vyhodnot_vyraz(Ttoken **token, SYMTB_itemptr_l locTab , bool assign)
{
	int chyba;
	char hell[MAX_RET];
	char pravidlo;
	int riadok,stlpec;
	Ttoken *t, *t2, ts, th;	//t2 je pomocny pointer na token a ts a th je pomocny token
	tdStack S,S2;		//zasobnik S, S2 pre generovanie instrukcii
    SYMTB_itemptr_l premenna;

	t=*token; //prvy token
	if (t->type == TKN_id && GST_search(global_symtb, t->attribute.string)) //kontrola ci id nie je funkcia
		return SYNTAX_ERR;

    if(t->type == TKN_id)  //je premenna deklarovana
    {
        premenna=LST_search(locTab, t->attribute.string);
        if (premenna == NULL)   //nebola deklarovana
            return SEMANT_ERR;
    }

	tdStackInit(&S2);
	if (tdStackPush(&S2, t) == COMPILER_ERR)
	{
		return COMPILER_ERR;
	}

	tdStackInit(&S);
	th.type='<';	//pomocny token ktory obsahuje zaciatok handle
	ts.type=TKN_EOL;	//pomocny token
	chyba=tdStackPush(&S, &ts);	//vlozi EOL($) na bottom zasobnika
	if (chyba)
	{
		tdStackDispose(&S);
		tdStackDispose(&S2);
		return COMPILER_ERR;
	}

    snprintf(hell,MAX_RET,"CLEARS");	//vycisti datovy zasobnik
    if (!add_instruction(&list, hell))
		return COMPILER_ERR;

	do
	{
		riadok=getTopTerm(&S);
        stlpec=getIndex(TokenType(t));
		if(stlpec < 0)	//nasiel token ktory necakal
		{
			*token=t;	//vratim ukazatel na token ktory nevyhovuje
			tdStackDispose(&S);
			tdStackDispose(&S2);
			return SYNTAX_ERR;
		}
		pravidlo=PrecTab[riadok][stlpec];
		switch (pravidlo)
		{
			case '=':	{
                            chyba=tdStackPush(&S, t);	//push token stlpca a precita dalsi symbol zo vstupu
                            if (chyba)
                            {
                                tdStackDispose(&S);
								tdStackDispose(&S2);
                                return COMPILER_ERR;
                            }
                            t=get_token();
					
							if ( t == NULL )
							{
                                tdStackDispose(&S);
								tdStackDispose(&S2);
								return LEXICAL_ERR;
							}

                            if (t->type == TKN_id && GST_search(global_symtb, t->attribute.string)) //je funkcia?
                            {
                                tdStackDispose(&S);
								tdStackDispose(&S2);
                                return SYNTAX_ERR;
                            }
							if (tdStackPush(&S2, t) == COMPILER_ERR)
    						{
								tdStackDispose(&S);
								tdStackDispose(&S2);
        						return COMPILER_ERR;
    						}
                            break;
                        }

			case '<':	{
                            tdStackActTop(&S);	//nastavi aktivitu na vrchol zasobnika
                            tdStackTop(&S, &t2);
                            int tmp5=getIndex(TokenType(t2));
                            if ( tmp5 >= 19 && tmp5 <=21 )	//na vrchole je neterminal
                            {
                                chyba=tdStackPreInsert(&S, &th);	//push '<' pred vrchol zasobnika*/
                                if (chyba)
                                {
                                    tdStackDispose(&S);
									tdStackDispose(&S2);
                                    return COMPILER_ERR;
                                }
                            }
                            else
                            {
                                chyba=tdStackPush(&S, &th);
                                if (chyba)
                                {
                                    tdStackDispose(&S);
									tdStackDispose(&S2);
                                    return COMPILER_ERR;
                                }
                            }

                            chyba=tdStackPush(&S, t);    //push token stlpca a precita dalsi symbol zo vstupu
                            if (chyba)
                            {
                                tdStackDispose(&S);
								tdStackDispose(&S2);
                                return COMPILER_ERR;
                            }

                            t=get_token();
			
							if ( t == NULL )
							{
                                tdStackDispose(&S);
								tdStackDispose(&S2);
								return LEXICAL_ERR;
							}

                            if (t->type == TKN_id && GST_search(global_symtb, t->attribute.string)) //bola funkcia deklarovana?
                            {
                                tdStackDispose(&S);
								tdStackDispose(&S2);
                                return SYNTAX_ERR;
                            }
							if (tdStackPush(&S2, t) == COMPILER_ERR)
                            {
                                tdStackDispose(&S);
                                tdStackDispose(&S2);
                                return COMPILER_ERR;
                            }
                            break;
                        }

			case '>':	{
							Ttoken *TknPole[MAXPOLE];	//pole ukazatelov na token
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
									fprintf(stderr,"maxpole error\n"); //TODO ERROR

								TknPole[i]=t2;	//ulozi ukazatel na pomocny token '<'
								tdStackSucc(&S);
								tdStackActCopy(&S, &t2);
							}
							rule=WhichRule(TknPole,locTab, poc);
							if (rule < 0)
                            {
                                tdStackDispose(&S);
								tdStackDispose(&S2);
                                switch (rule)
                                {
                                    case -2: return SYNTAX_ERR;
                                    case -3: return SEMANT_ERR;
                                    case -4: return TYPE_ERR;
                                    default: return SYNTAX_ERR;
                                }
                            }

							int v=DoRule(&S, locTab, rule, assign);
							if (v != 0)
							{   if (v == COMPILER_ERR)
                                {
                                    tdStackDispose(&S);
									tdStackDispose(&S2);
                                    return COMPILER_ERR;
                                }
                                else if (v == OTHER_SEMANT_ERR)
                                {
                                    tdStackDispose(&S);
									tdStackDispose(&S2);
                                    return OTHER_SEMANT_ERR;
                                }
                                else if (v == TYPE_ERR)
                                {
                                    tdStackDispose(&S);
									tdStackDispose(&S2);
                                    return TYPE_ERR;
                                }
								else if(v == -1)
                                {
                                    tdStackDispose(&S);
                                    tdStackDispose(&S2);
                                    return 9;
                                }
							}
                            break;
                        }

			case 'E':	tdStackDispose(&S);
						tdStackDispose(&S2);
                        return SYNTAX_ERR;
			default: break;
		}
	}while( stlpec != 17 || riadok != 17 );

	*token=t;	//vratim ukazatel na dalsi token

	tdStackDispose(&S);
	tdStackDispose(&S2);

	snprintf(hell,MAX_RET,"POPS LF@$result");    //presunie vyslednu hodnotu do result premennej
    if(!add_instruction(&list, hell))
        return COMPILER_ERR;


	return 0;	//vsetko v poriadku
}

int DoRule(tdStack *S, SYMTB_itemptr_l locTab, int rule, bool assign)
{
    SYMTB_itemptr_l loc;
	char result_type;	//premenna znaciaca vysledny neterminal
	int chyba;
	char ret[MAX_RET];	//pomocny retazec na generovanie instrukcii
	char *str;		//pomocny retazec na generovanie instrukcii
	//char ins_typ;	//urcuje typ vysledneho vyrazu v instrukciach	('i','s','d','b')
	Ttoken *new, *t1, *t2, *t3, *t4;
	tdStackActTop(S);
	if (rule <= 12 && rule >=0)
    {
        tdStackActCopy(S, &t4);	// op2
		tdStackPred(S);
		tdStackActCopy(S, &t3);	// operator
        tdStackPred(S);
        tdStackActCopy(S, &t2);	// op1
        tdStackPred(S);
		tdStackActCopy(S, &t1);	// '<' handle
    }
    else
    {
        tdStackActCopy(S, &t2);	// id/int/double/string
        tdStackPred(S);
		tdStackActCopy(S, &t1); // '<' handle
    }

	if (rule == 1)	// '*'I->I*I, D->D*D, D->D*I, D->I*D
	{
		//Oba operandy su typu double
		if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
			 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
		{
			result_type='D';
			ins_typ='d';
			snprintf(ret,MAX_RET,"MULS");	//vynasobi
           	if(!add_instruction(&list, ret))
               	return COMPILER_ERR;
		}
		//oba operandy su typu int
		else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
		{
			result_type='I';
			ins_typ='i';
			snprintf(ret,MAX_RET,"MULS");	//vynasobi
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
		}
		//jeden operand je double a druhy int
		else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
	               ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
			     ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
		{
			result_type='D';
			ins_typ='d';
			if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
			{
				snprintf(ret,MAX_RET,"INT2FLOATS");	//pretypuj druhy int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
			}
			else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
            {
				snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
			snprintf(ret,MAX_RET,"MULS");    //vynasobi
            if(!add_instruction(&list, ret))
            	return COMPILER_ERR;
		}
		else
			return TYPE_ERR;	//semanticka chyba
	}
	else if (rule == 2)	// '+'	D->D+D, I->I+I, D->I+D, D->D+I, S->S+S
	{
        //Oba operandy su typu double
        if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        {
            result_type='D';
			ins_typ='d';
			snprintf(ret,MAX_RET,"ADDS");  //scita
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        //oba operandy su typu int
        else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        {
            result_type='I';
			ins_typ='i';
			snprintf(ret,MAX_RET,"ADDS");  //scita
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        //jeden operand je double a druhy int
        else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
                 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        {
            result_type='D';
			ins_typ='d';
			if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
            {
                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
            else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
            {
                snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
            snprintf(ret,MAX_RET,"ADDS");    //scita
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
		//oba operandy su typu string
		else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
		{
			result_type='S';
			ins_typ='s';
			snprintf(ret,MAX_RET,"POPS GF@$tmp2"); //do tmp2 ulozi operand 2
    		if (!add_instruction(&list, ret))
        		return COMPILER_ERR;

			snprintf(ret,MAX_RET,"POPS GF@$tmp1"); //do tmp1 ulozi operand 1
            if (!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"CONCAT GF@$tmp3 GF@$tmp1 GF@$tmp2");  //konkatenacia
            if(!add_instruction(&list, ret))
             	return COMPILER_ERR;

			snprintf(ret,MAX_RET,"PUSHS GF@$tmp3"); //ulozi vysledok naspat na zasobnik
            if (!add_instruction(&list, ret))
                return COMPILER_ERR;
		}
		else
			return TYPE_ERR;
	}
    else if (rule == 3)	// '-'	D->D-D, I->I-I, D->D-I, D->I-D
    {
        //Oba operandy su typu double
        if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        {
            result_type='D';
			ins_typ='d';
			snprintf(ret,MAX_RET,"SUBS");  //odcita
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

        }
        //oba operandy su typu int
        else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        {
            result_type='I';
			ins_typ='i';
			snprintf(ret,MAX_RET,"SUBS");  //odcita
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        //jeden operand je double a druhy int
        else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
                 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
              	   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        {
            result_type='D';
			ins_typ='d';
			if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
            {
                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
            else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
            {
                snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
            snprintf(ret,MAX_RET,"SUBS");    //odcita
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        else
            return TYPE_ERR;
    }
    else if (rule == 4)	// '/'	D->D/D, D->I/I, D->D/I, D->I/D
    {
        //Oba operandy su typu double
        if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        {
            result_type='D';
			ins_typ='d';
			snprintf(ret,MAX_RET,"DIVS");    //vydeli
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        //oba operandy su typu int
        else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        {
            result_type='D';
			ins_typ='d';
			snprintf(ret,MAX_RET,"INT2FLOATS");    //pretypuje druhy na double
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"POPS GF@$tmp2");    //ulozi druhy operand do tmp2
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"INT2FLOATS");    //pretypuje prvy na double
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    //vrati operand 2 na zasobnik
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"DIVS");    //vydeli
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        //jeden operand je double a druhy int
        else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             	 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             	   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        {
            result_type='D';
			ins_typ='d';
			if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
            {
                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
            else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
            {
                snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
            }
            snprintf(ret,MAX_RET,"DIVS");    //vydeli
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        else
            return TYPE_ERR;
    }
    else if (rule == 5) // '\' I->I\I
    {
        //oba operandy su typu int
        if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        {
            result_type='I';
			ins_typ='i';
			snprintf(ret,MAX_RET,"INT2FLOATS");    //pretypuje druhy operand
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"POPS GF@$tmp2");   //ulozi operand 2 do tmp2
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"INT2FLOATS");    //pretypuje prvy operand
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    //ulozi druhy spat na zasobnik
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"DIVS");    //vydeli
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;

			snprintf(ret,MAX_RET,"FLOAT2INTS");    //pretypuje vysledok na int
            if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
        else
            return TYPE_ERR;
    }
    else if (rule == 6)	// '<'	D->D<D, I->I<I, D->D<I, D->I<D, S->S<S
    {
		if (assign)
		{
				return OTHER_SEMANT_ERR;
		}
		else
		{
			//Oba operandy su typu double
        	if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             	 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        	{
            	result_type='D';
				ins_typ='b';
				snprintf(ret,MAX_RET,"LTS");    // mensie nez
            	if(!add_instruction(&list, ret))
                	return COMPILER_ERR;
        	}
        	//oba operandy su typu int
        	else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        	{
            	result_type='I';
				ins_typ='b';
				snprintf(ret,MAX_RET,"LTS");    //mensie nez
            	if(!add_instruction(&list, ret))
                	return COMPILER_ERR;
        	}
        	//jeden operand je double a druhy int
        	else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   	   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             		 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             		   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        	{
            	result_type='D';
				ins_typ='b';
				if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
            	{
                	snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                	if(!add_instruction(&list, ret))
                    	return COMPILER_ERR;
            	}
            	else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
            	{
                	snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                	if(!add_instruction(&list, ret))
                    	return COMPILER_ERR;

                	snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                	if(!add_instruction(&list, ret))
                    	return COMPILER_ERR;

                	snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                	if(!add_instruction(&list, ret))
                    	return COMPILER_ERR;
            	}
            	snprintf(ret,MAX_RET,"LTS");    //mensie nez
            	if(!add_instruction(&list, ret))
                	return COMPILER_ERR;
        	}
			//oba operandy su typu string
        	else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  	  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
        	{
            	result_type='S';
				ins_typ='b';
				snprintf(ret,MAX_RET,"LTS");    //mensie nez
            	if(!add_instruction(&list, ret))
                	return COMPILER_ERR;
        	}
        	else
            	return TYPE_ERR;
		}
    }
    else if (rule == 7)	// '>'	D->D>D, I->I>I, D->D>I, D->I>D, S->S>S
    {
		if (assign)
		{
				return OTHER_SEMANT_ERR;
		}
		else
		{
			//Oba operandy su typu double
        	if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             	 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        	{
            	result_type='D';
				ins_typ='b';
				snprintf(ret,MAX_RET,"GTS");    // vacsie nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//oba operandy su typu int
        	else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        	{
            	result_type='I';
				ins_typ='b';
				snprintf(ret,MAX_RET,"GTS");   // vacsie nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//jeden operand je double a druhy int
        	else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   	   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             		 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             		   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        	{
            	result_type='D';
				ins_typ='b';
				if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
                {
                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
                {
                    snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                snprintf(ret,MAX_RET,"GTS");    //vacsie nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
			//oba operandy su typu string
        	else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  	  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
        	{
            	result_type='S';
				ins_typ='b';
				snprintf(ret,MAX_RET,"GTS");    // vacsie nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	else
            	return TYPE_ERR;
		}
    }
    else if (rule == 8)	// '='	D->D=D, I->I=I, D->D=I, D->I=D, S->S=S
    {
		if (assign)
		{
				return OTHER_SEMANT_ERR;
		}
		else
		{
			//Oba operandy su typu double
        	if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             	 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        	{
            	result_type='D';
				ins_typ='b';
				snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//oba operandy su typu int
        	else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        	{
            	result_type='I';
				ins_typ='b';
				snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//jeden operand je double a druhy int
        	else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   	   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             		 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             		   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        	{
            	result_type='D';
				ins_typ='b';
				if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
                {
                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
                {
                    snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                snprintf(ret,MAX_RET,"EQS");    //rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

        	}
			//oba operandy su typu string
        	else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  	  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
        	{
            	result_type='S';
				ins_typ='b';
				snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	else
            	return TYPE_ERR;
		}
    }
    else if (rule == 9)	// '<='	D->D<=D, I->I<=I, D->D<=I, D->I<=D, S->S<=S
    {
		if (assign)
		{
				return OTHER_SEMANT_ERR;
		}
		else
		{
			//Oba operandy su typu double
        	if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             	 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        	{
            	result_type='D';
				ins_typ='b';
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"LTS");    //mensi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"EQS");   // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//oba operandy su typu int
        	else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        	{
            	result_type='I';
				ins_typ='b';
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"LTS");    //mensi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//jeden operand je double a druhy int
        	else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   	   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             		 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             		   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        	{
            	result_type='D';
				ins_typ='b';
				if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
                {
                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
                {
					snprintf(ret,MAX_RET,"POPS GF@$tmp2");  //uloz druhy operand do tmp2
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

					snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

					snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");  //vrat op2 spat
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
              	snprintf(ret,MAX_RET,"POPS GF@$tmp2");   // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"LTS");    //mensi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                   	return COMPILER_ERR;
        	}
			//oba operandy su typu string
        	else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  	  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
        	{
            	result_type='S';
				ins_typ='b';
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"LTS");    //mensi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	else
            	return TYPE_ERR;
		}
    }
    else if (rule == 10)	// '>='		D->D>=D, I->I>=I, D->D>=I, D->I>=D, S->S>=S
    {
		if (assign)
		{
				return OTHER_SEMANT_ERR;
		}
		else
		{
			//Oba operandy su typu double
        	if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             	 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        	{
            	result_type='D';
				ins_typ='b';
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"GTS");    //vacsi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

        	}
        	//oba operandy su typu int
        	else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        	{
            	result_type='I';
				ins_typ='b';
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"GTS");    //vacsi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//jeden operand je double a druhy int
        	else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   	   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             		 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             		   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        	{
            	result_type='D';
				ins_typ='b';
				if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
                {
                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
                {
                    snprintf(ret,MAX_RET,"POPS GF@$tmp2");  //uloz druhy operand do tmp2
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");  //vrat op2 spat
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"GTS");    //vacsi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
			//oba operandy su typu string
        	else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  	  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
        	{
            	result_type='S';
				ins_typ='b';
				snprintf(ret,MAX_RET,"POPS GF@$tmp2");    // uloz operand 2 do tmp2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"POPS GF@$tmp");    // uloz operand 1 do tmp
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vrati naspat op1
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");    // vrati naspat op2
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"GTS");    //vacsi nez
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp");    // vlozi op1 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"PUSHS GF@$tmp2");   // vlozi op2 na zasobnik
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"ORS");    // or
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	else
            	return TYPE_ERR;
		}
    }
    else if (rule == 11)	// '<>	D->D<>D, I->I<>I, D->D<>I, D->I<>D, S->S<>S'
    {
		if (assign)
		{
				return OTHER_SEMANT_ERR;
		}
		else
		{
			//Oba operandy su typu double
        	if ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
             	 ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) )
        	{
            	result_type='D';
				ins_typ='b';
				snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"NOTS");    // not
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//oba operandy su typu int
        	else if ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
                  	  ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) )
        	{
            	result_type='I';
				ins_typ='b';
				snprintf(ret,MAX_RET,"EQS");    // rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"NOTS");    // not
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	//jeden operand je double a druhy int
        	else if( ( ((TokenType(t2) == TKN_dbl) || (TokenType(t2) == 'D')) &&
                   	   ((TokenType(t4) == TKN_int) || (TokenType(t4) == 'I')) ) ||
             		 ( ((TokenType(t2) == TKN_int) || (TokenType(t2) == 'I')) &&
             		   ((TokenType(t4) == TKN_dbl) || (TokenType(t4) == 'D')) ) )
        	{
            	result_type='D';
				ins_typ='b';
				if( (TokenType(t2) == 'D') && (TokenType(t4) == 'I') )
                {
                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj druhy int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                else if( (TokenType(t2) == 'I') && (TokenType(t4) == 'D') )
                {
                    snprintf(ret,MAX_RET,"POPS GF@$tmp");    //ulozi druhy operand do docasnej premennej
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"INT2FLOATS");  //pretypuj prvy operand int na double
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;

                    snprintf(ret,MAX_RET,"PUSHS GF@$tmp");  //vrati docasnu premennu spat na zasobnik
                    if(!add_instruction(&list, ret))
                        return COMPILER_ERR;
                }
                snprintf(ret,MAX_RET,"EQS");    //rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

				snprintf(ret,MAX_RET,"NOTS");    //not
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
			//oba operandy su typu string
        	else if ( ((TokenType(t2) == TKN_str) || (TokenType(t2) == 'S')) &&
                  	  ((TokenType(t4) == TKN_str) || (TokenType(t4) == 'S')) )
        	{
            	result_type='S';
				ins_typ='b';
				snprintf(ret,MAX_RET,"EQS");    //rovne
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;

                snprintf(ret,MAX_RET,"NOTS");    //not
                if(!add_instruction(&list, ret))
                    return COMPILER_ERR;
        	}
        	else
            	return TYPE_ERR;
		}
    }
    else if (rule == 12)	// '(id)', '(int)', '(str)', '(D)', '(I)', '(S)'
    {
   		if ((TokenType(t3) == TKN_int) || (TokenType(t3) == 'I'))
		{
			result_type='I';
			ins_typ='i';
		}
		else if ((TokenType(t3) == TKN_dbl) || (TokenType(t3) == 'D'))
		{
			result_type='D';
			ins_typ='d';
		}
		else if ((TokenType(t3) == TKN_str) || (TokenType(t3) == 'S'))
		{
			result_type='S';
            ins_typ='s';
		}
		else
			return TYPE_ERR;
    }
	else if (rule == 13)	//rule pre id
	{
	    loc=LST_search(locTab,t2->attribute.string);
        if (loc == NULL)
            return -3;  //premenna nie je deklarovana ale toto by nemalo uz nastat
        else
        {
            switch (loc->type)
            {
                case 'i':{
							result_type='I';
                            ins_typ='i';
							break;
						 }
                case 'd':{
							result_type='D';
							ins_typ='d';
							break;
						 }
                case 's':{
							result_type='S';
							ins_typ='s';
   							break;
						 }
                default:break;
            }
			snprintf(ret,MAX_RET,"PUSHS LF@%s",t2->attribute.string);    //vlozi danu premennu na zasobnik
			if(!add_instruction(&list, ret))
                return COMPILER_ERR;
        }
	}
    else if (rule == 14)	//rule pre 'I'->int
    {
        if (TokenType(t2) == TKN_int || TokenType(t2) == 'I')
        {
            result_type='I';
            ins_typ='i';

			snprintf(ret,MAX_RET,"PUSHS int@%d",t2->attribute.integer);
			if (!add_instruction(&list, ret))	//push int na datovy zasobnik
				return COMPILER_ERR;
        }
        else
            return TYPE_ERR;
    }
	else if (rule == 15)	//rule pre 'D'->double
    {
        if (TokenType(t2) == TKN_dbl || TokenType(t2) == 'D')
        {
            result_type='D';
            ins_typ='d';

			snprintf(ret,MAX_RET,"PUSHS float@%g",t2->attribute.dble);
			if(!add_instruction(&list, ret))	//push dbl na datovy zasobnik
				return COMPILER_ERR;
        }
        else
            return TYPE_ERR;
    }
    else if (rule == 16)	//rule pre 'S'->str
    {
        if (TokenType(t2) == TKN_str || TokenType(t2) == 'S')
        {
            result_type='S';
            ins_typ='s';

			int dlzka=strlen(t2->attribute.string);
			if((str=malloc((14 + dlzka)*sizeof(char))) == NULL)
			{
				return COMPILER_ERR;
			}
            snprintf(str,dlzka+14,"PUSHS string@%s",t2->attribute.string);
            if(!add_instruction(&list, str))
            {
				free(str);
                return COMPILER_ERR;
            }
			free(str);	//uvolni docasnu pamat pre string
        }
        else
            return TYPE_ERR;
    }

    if (rule < 13)
    {
        for(int i=0; i<4;i++)
            tdStackDeleteTop(S);
    }
    else
    {
        for(int i=0;i<2;i++)
            tdStackDeleteTop(S);

    }

    new=malloc(sizeof(struct token));
	if (new == NULL)
	{
		return COMPILER_ERR;
	}
	new->type =result_type;
	chyba=tdStackPush(S, new);
	if (chyba)
	{
		free(new);
        return COMPILER_ERR;
	}
	return 0;	//OK
}

int WhichRule(Ttoken **pole,SYMTB_itemptr_l locTab, int max)
{
    SYMTB_itemptr_l loc=NULL;

    if (pole[0]->type != '<')	//ak nie je prve zaciatok handle, niekde sa stala chyba
        return -2;  //SYNTAX_ERR
	if (max == 4)
	{
		if ((pole[1]->type == TKN_leftpar && pole[3]->type == TKN_rightpar) || (pole[1]->type == 'I' && pole[3]->type == 'I') ||	//(I/D/S) IoI
			(pole[1]->type == 'D' && pole[3]->type == 'D') || (pole[1]->type == 'S' && pole[3]->type == 'S') ||	//DoD SoS
			(pole[1]->type == 'I' && pole[3]->type == 'D') || (pole[1]->type == 'D' && pole[3]->type == 'I'))	//IoD DoI
		{
			switch (getIndex(TokenType(pole[2])))
			{
				case 0: return 1;   break;	// '*'
				case 1: return 2;	break;  // '+'
				case 2: return 3;	break;  // '-'
				case 3: return 4;	break;  // '/'
				case 4: return 5;	break;  // '\'
				case 5: return 6;	break;  // '<'
				case 6: return 7;	break;  // '>'
				case 7: return 8;	break;  // '='
				case 8: return 9;	break;  // '<='
				case 9: return 10;	break;  // '>='
				case 10: return 11;	break;  // '<>'
				case 19: return 12;	break;  // 'I'
				case 20: return 12;	break;  // 'D'
				case 21: return 12; break;  // 'S'
				default: return -2;	break;//Ziadne pravidlo SYNTAX_ERR
			}
		}
		return -4;  //typy nesedia
	}
	else if (max == 2)
	{
		switch (getIndex(TokenType(pole[1])))
		{	//id premennej
			case 13: {
                        loc=LST_search(locTab,pole[1]->attribute.string);
                        if (loc == NULL)
                            return -3;  //premenna nie je deklarovana
                        else
                            return 13;
                    }
			case 14: return 14; // int
			case 15: return 15;	// double
			case 16: return 16;	// string
			default: return -2;	//ziadne pravidlo
		}
	}
	else
    {
		return -2;
    }
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
	if (type == TKN_EOL || type == TKN_smcolon || type==KWD_then)	// $
        return 17;
	if (type == '<')	//zaciatok handle
		return 18;
	if (type == 'I')	//neterminal int
		return 19;
	if (type == 'D')	//neterminal double
		return 20;
	if (type == 'S')	//neterminal string
		return 21;
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
		if (tmp->TokenPtr->type == 'I' || tmp->TokenPtr->type == 'D' || tmp->TokenPtr->type == 'S')
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

void tdStackActBottom(tdStack *S)
{
	S->Act=S->Bottom;
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
		if (tmp->TokenPtr->type == 'I' || tmp->TokenPtr->type == 'D' || tmp->TokenPtr->type == 'S')
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
            if (tmp->TokenPtr->type == 'I' || tmp->TokenPtr->type == 'D' || tmp->TokenPtr->type == 'S')
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
