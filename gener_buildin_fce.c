/**
 * Názov: gener_buildin_fce.c
 * Projekt IFJ
 * Prekladač zdrojového jazyka IFJ17 do cieľového jazyka IFJcode17
 * Autori:
 * Maroš Holko			xholko01
 * Denis Leitner		xleitn02
 * Vlastimil Rádsetoulal	xradse00
 * Michal Štábel		xstabe00
 */


#include"gener_buildin_fce.h"

#define MAX 150


int build_all(void)
{
	if(generate_length())
		return COMPILER_ERR;
	if(generate_substr())
		return COMPILER_ERR;
	if(generate_asc())
		return COMPILER_ERR;
	if(generate_chr())
		return COMPILER_ERR;
	return 0;
}

int generate_length(void)
{
	char ret[MAX];
	snprintf(ret,MAX,"LABEL length");	//navestie
	if(!add_instruction(&list, ret))	
		return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@s"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@s LF@$par0");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@return");   //vytvori navratovu premennu
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"STRLEN LF@return LF@s");   //zisti dlzku retazca
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
		
	snprintf(ret,MAX,"RETURN");   //return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	return 0;
}

int generate_substr(void)
{
	char ret[MAX];
	snprintf(ret,MAX,"LABEL substr");   //navestie
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
	
	snprintf(ret,MAX,"DEFVAR LF@s"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@s LF@$par0");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@i"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@i LF@$par1");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@n"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@n LF@$par2");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@return");   //vytvori navratovu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@substrtmp");   //vytvori pomocnu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@substrtmp2");   //vytvori pomocnu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@substrtmp3");   //vytvori pomocnu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@substrtmp4");   //vytvori pomocnu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"STRLEN LF@substrtmp LF@s");   //ulozi dlzku s do tmp
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
//###############################################	je s prazdne??
//###############################################
	snprintf(ret,MAX,"EQ LF@substrtmp2 LF@substrtmp int@0");   //	tmp2= true ak je strlen(s)==0
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFEQ emptystring LF@substrtmp2 bool@true");  //skoc na emptystring ak true
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
//############################### je i<=0 ??
	snprintf(ret,MAX,"LT LF@substrtmp2 LF@i int@0");   //tmp2= true ak i<0
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"EQ LF@substrtmp3 LF@i int@0");   //tmp3= true ak i==0
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"OR LF@substrtmp2 LF@substrtmp2 LF@substrtmp3");   //tmp2= tmp2 || tmp3
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFEQ emptystring LF@substrtmp2 bool@true");   //skoc na emptystring ak true
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
//############################################## je n<0 ??
//##############################################
	snprintf(ret,MAX,"LT LF@substrtmp2 LF@n int@0");   //tmp2=true ak n<0
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFEQ allfromi LF@substrtmp2 bool@true");   //skoc na allfromi ak true
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
//########################### je n > strlen(s)-i ??
	snprintf(ret,MAX,"SUB LF@substrtmp3 LF@substrtmp LF@i");   //tmp3= strlen(s)-i
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"GT LF@substrtmp2 LF@n LF@substrtmp3");   //tmp2=true ak n > strlen(s)-i
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFEQ allfromi LF@substrtmp2 bool@true");   //skoc na allfromi ak true
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
//############################################## to co ma funkcia robit 
//##############################################
	snprintf(ret,MAX,"SUB LF@substrtmp2 LF@i int@1");   //tmp2= i-1 //tmp2 sluzi na indexovanie 
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
	
	snprintf(ret,MAX,"MOVE LF@return string@");   //do return ulozi prazdny retazec
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LABEL cycle1");   //navestie zaciatku cyklu1
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
	
	snprintf(ret,MAX,"SUB LF@substrtmp4 LF@substrtmp2 LF@i");   //tmp4= tmp2-i
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"ADD LF@substrtmp4 LF@substrtmp4 int@1");   //tmp4--
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LT LF@substrtmp3 LF@substrtmp4 LF@n");   //tmp3= tmp4 < n 
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFNEQ endcycle1 LF@substrtmp3 bool@true");	//vyskoci z cyklu ak uz precital n znakov
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"GETCHAR LF@substrtmp4 LF@s LF@substrtmp2");   //tmp4=s[tmp2]	//tmp4 mi uz nie je treba
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"CONCAT LF@return LF@return LF@substrtmp4");   //spoji return retazec s tmp4
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"ADD LF@substrtmp2 LF@substrtmp2 int@1");   //zvysi index
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;	

	snprintf(ret,MAX,"JUMP cycle1");   //skoci na zaciatok cyklu1
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LABEL endcycle1");   //navestie konca cykla1
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"RETURN");   //return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

//########################################## vrat vsetko od pozicie i
	snprintf(ret,MAX,"LABEL allfromi");   //navestie
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
	
	snprintf(ret,MAX,"SUB LF@substrtmp2 LF@i int@1");   //tmp2=i-1	//tmp2 je index
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@return string@");   //vlozi prazdny string do return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LABEL cycle2");   //navestie zaciatku cyklu2
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LT LF@substrtmp4 LF@substrtmp2 LF@substrtmp");   //tmp4=tmp2 < strlen(s)
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFNEQ endcycle2 LF@substrtmp4 bool@true");   //skoci von z cyklu2
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"GETCHAR LF@substrtmp4 LF@s LF@substrtmp2");   //tmp4=s[tmp2]	//tmp4 uz nie je potrebne
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"CONCAT LF@return LF@return LF@substrtmp4");   //spoji return s tmp4
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"ADD LF@substrtmp2 LF@substrtmp2 int@1");   //zvysi index
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMP cycle2");   //skoci na zaciatok cyclu2
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LABEL endcycle2");   //navestie konca cyklu2
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"RETURN");   //return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

//#############################################	//vrat prazdny string
	snprintf(ret,MAX,"LABEL emptystring");   //navestie
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@return string@");   //vrati prazdny retazec
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
	
	snprintf(ret,MAX,"RETURN");   //return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	return 0;
}

int generate_asc(void)
{
	char ret[MAX];
	snprintf(ret,MAX,"LABEL asc");   //navestie
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
		
	snprintf(ret,MAX,"DEFVAR LF@s"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@s LF@$par0");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@i"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@i LF@$par1");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@return");   //vytvori navratovu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
		
	snprintf(ret,MAX,"DEFVAR LF@asctmp");   //vytvori pomocnu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@asctmp2");   //vytvori pomocnu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"STRLEN LF@asctmp LF@s");   //ulozi dlzku retazca do asctmp
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

//############################################## overovanie medzi
	snprintf(ret,MAX,"GT LF@asctmp2 LF@i LF@asctmp");   // tmp2= i > strlen(s) ??
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFEQ tmpout LF@asctmp2 bool@true");   //skoci na navestie tmpout ak je i>strlen(s)
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"LT LF@asctmp2 LF@i int@1");   //	tmp2= i<1 ??
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"JUMPIFEQ tmpout LF@asctmp2 bool@true");   //skoci na navestie tmpout ak je i<1
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

//############################################## to co ma robit funkcia
	snprintf(ret,MAX,"SUB LF@asctmp LF@i int@1");   // i-1 ulozi do tmp
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"STRI2INT LF@return LF@s LF@asctmp");   //do return ulozi dany znak
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"RETURN");   //return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

//############################################## mimo medze
	snprintf(ret,MAX,"LABEL tmpout");   //navestie ked i je mimo rozsah stringu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@return int@0");   //ulozi 0 do return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"RETURN");   //vrati 0
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	return 0;
}

int generate_chr(void)
{
	char ret[MAX];
	snprintf(ret,MAX,"LABEL chr");   //navestie
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@i"); 
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"MOVE LF@i LF@$par0");
    if(!add_instruction(&list, ret))            
        return COMPILER_ERR;

	snprintf(ret,MAX,"DEFVAR LF@return");   //vytvori navratovu premennu
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;
	
	snprintf(ret,MAX,"INT2CHAR LF@return LF@i");   //prevedie int v i na char 
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	snprintf(ret,MAX,"RETURN");   //return
    if(!add_instruction(&list, ret))
        return COMPILER_ERR;

	return 0;
}
