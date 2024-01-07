%define parse.trace
%define parse.error verbose

%define api.pure full
%param {yyscan_t scanner}
%parse-param {int* result}

%code requires {
    typedef void * yyscan_t;
}

%{
#include <stdio.h>

#include "parser.h"
#include "lexer.h"

void yyerror(yyscan_t scanner, int* result, const char * err);
%}

%union {
    int ival;
}

%token <ival> NUMBER

%left '+' '-'
%left '*' '/' '%'
%precedence UMINUS UPLUS

%token END 0

%type <ival> exp

%%

program: %empty
 | program exp END { *result = $2; YYACCEPT; }
 ;

exp: NUMBER { $$ = $1; }
   | exp '+' exp { $$ = $1 + $3; }
   | exp '-' exp { $$ = $1 - $3; }
   | exp '*' exp { $$ = $1 * $3; }
   | exp '/' exp { $$ = $1 / $3; }
   | exp '%' exp { $$ = $1 % $3; }
   | '(' exp ')' { $$ = $2; }
   | '-' exp %prec UMINUS { $$ = - $2; }
   | '+' exp %prec UPLUS { $$ = + $2; }
   ;

%%

void yyerror(yyscan_t scanner, int* result, const char * err) {
  fprintf(stderr, "PARSE ERROR: %s\n", err);
}