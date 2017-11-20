test: parser.c parser.h  precedSA.c precedSA.h  gettoken.c gettoken.h symtable.c symtable.h instructions.h instructions.c
	gcc -std=c99 -Werror -Wextra gettoken.c parser.c precedSA.c symtable.c instructions.c -o test
