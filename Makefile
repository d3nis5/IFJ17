test: parser.c parser.h  precedsa.c precedsa.h symtable.c symtable.h instructions.h instructions.c scanner.c scanner.h gener_buildin_fce.c gener_buildin_fce.h token.c token.h
	gcc -std=c99  -Wextra parser.c precedsa.c gener_buildin_fce.c symtable.c instructions.c scanner.c token.c -o test
