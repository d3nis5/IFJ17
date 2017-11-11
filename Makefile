test: parser.c precedSA.c
	gcc -std=c99 -Werror -Wextra parser.c precedSA.c -o test
