#include "symtable.h"

#ifndef TOKEN_H
#define TOKEN_H


typedef enum token_type
{ 
	TKN_id,			/* identifikator */

	/* DATOVE TYPY */

	TKN_int,		/* integer */
	TKN_dbl,		/* double */
	TKN_str,		/* string */

	/* OPERATORY */

	TKN_plus,		/* + */
	TKN_minus,		/* - */
	TKN_star,		/* * */
	TKN_slash,		/* / */
	TKN_backslash,	/* \ */
	TKN_gt,			/* > */
	TKN_less,		/* < */
	TKN_leq,		/* <= */
	TKN_geq,		/* >= */
	TKN_neq,		/* <> */
	TKN_eq,			/* = */

	/* OSTATNE */

	TKN_smcolon,	/* ; */
	TKN_colon,		/* , */
	TKN_leftpar,	/* ( */
	TKN_rightpar,	/* ) */
	TKN_EOL,		/* \n */
	TKN_EOF,		/* EOF */

	/* KLUCOVE SLOVA */

	KWD_as,
	KWD_asc,
	KWD_declare,
	KWD_dim,
	KWD_do,
	KWD_double,
	KWD_else,
	KWD_end,
	KWD_chr,
	KWD_function,
	KWD_if,
	KWD_input,
	KWD_integer,
	KWD_length,
	KWD_loop,
	KWD_print,
	KWD_return,
	KWD_scope,
	KWD_string,
	KWD_substr,
	KWD_then,
	KWD_while,

	/* REZERVOVANE KLUCOVE SLOVA */

	KWD_and,
	KWD_boolean,
	KWD_continue,
	KWD_elseif,
	KWD_exit,
	KWD_false,
	KWD_for,
	KWD_next,
	KWD_not,
	KWD_or,
	KWD_shared,
	KWD_static,
	KWD_true,

} TKN_type;


/* Struktura popisujuca token */

typedef struct token
{
	TKN_type type;
	union
	{
		int integer;			/* hodnota integer */
		double dble;			/* hodnota double */
		char *string;			/* retazec */
		SYMTB_itemptr st_ptr;	/* ukazatel do tabulky symbolov */
	} attribute;

} Ttoken;


#endif /* TOKEN_H */
