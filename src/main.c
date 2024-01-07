#include <stdio.h>

#include "parser.h"
#include "lexer.h"

int main(int argc, char* argv[]) {
    int result = 0;
    printf("> ");

    yyscan_t scanner;
    yylex_init(&scanner);
    yyparse(scanner, &result);
    yylex_destroy(scanner);

    printf("result = %d\n", result);
    return 0;
}