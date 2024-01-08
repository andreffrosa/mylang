%define parse.trace
%define parse.error verbose

%define api.pure full
//%locations
%parse-param {yyscan_t scanner} {struct ASTNode** ast}
%lex-param {yyscan_t scanner}

%code requires {
  typedef void * yyscan_t;
  typedef struct ASTNode ASTNode;
}

%code provides {
  void yyerror(yyscan_t scanner, struct ASTNode** ast, const char * s, ...);
}

%{
#include <stdio.h>
#include <stdarg.h>

#include "parser.h"
#include "lexer.h"

#include "ast/ast.h"
%}

%union {
    int ival;
    ASTNode* ast_node;
}

%token <ival> NUMBER

%left '+' '-'
%left '*' '/' '%'
%precedence UMINUS UPLUS SET_POSITIVE SET_NEGATIVE

%token OPEN_ABS CLOSE_ABS

%left BITWISE_OR BITWISE_AND BITWISE_XOR L_SHIFT R_SHIFT
%precedence BITWISE_NOT

%token END 0

%type <ast_node> exp

%%

program: %empty
 | program exp END { *ast = $2; YYACCEPT; }
 ;

exp: NUMBER { $$ = newASTNumber($1); }
   | exp '+' exp { $$ = newASTBinaryOP(AST_ADD, $1, $3); }
   | exp '-' exp { $$ = newASTBinaryOP(AST_SUB, $1, $3); }
   | exp '*' exp { $$ = newASTBinaryOP(AST_MUL, $1, $3); }
   | exp '/' exp { $$ = newASTBinaryOP(AST_DIV, $1, $3); }
   | exp '%' exp { $$ = newASTBinaryOP(AST_MOD, $1, $3); }
   | '(' exp ')' { $$ = $2; }
   | '-' exp %prec UMINUS { $$ = newASTUnaryOP(AST_USUB, $2); }
   | '+' exp %prec UPLUS { $$ = newASTUnaryOP(AST_UADD, $2); }
   | exp BITWISE_OR exp { $$ = newASTBinaryOP(AST_BITWISE_OR, $1, $3); }
   | exp BITWISE_AND exp { $$ = newASTBinaryOP(AST_BITWISE_AND, $1, $3); }
   | exp BITWISE_XOR exp { $$ = newASTBinaryOP(AST_BITWISE_XOR, $1, $3); }
   | exp L_SHIFT exp { $$ = newASTBinaryOP(AST_L_SHIFT, $1, $3); }
   | exp R_SHIFT exp { $$ = newASTBinaryOP(AST_R_SHIFT, $1, $3); }
   | BITWISE_NOT exp { $$ = newASTUnaryOP(AST_BITWISE_NOT, $2); }
   | OPEN_ABS exp CLOSE_ABS { $$ = newASTUnaryOP(AST_ABS, $2); }
   | SET_POSITIVE exp { $$ = newASTUnaryOP(AST_SET_POSITIVE, $2); }
   | SET_NEGATIVE exp { $$ = newASTUnaryOP(AST_SET_NEGATIVE, $2); }
   ;

%%

void yyerror(yyscan_t scanner, struct ASTNode** ast, const char * s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "(line %d) PARSE ERROR: ", yyget_lineno(scanner));
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}
