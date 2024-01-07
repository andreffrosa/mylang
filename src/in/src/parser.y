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
%precedence UMINUS UPLUS

%token END 0

%type <ast_node> exp

%%

calclist: %empty /* nothing */
 | calclist exp END { *ast = $2; YYACCEPT; }
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
   ;

%%

void yyerror(yyscan_t scanner, struct ASTNode** ast, const char * s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "(line %d) PARSE ERROR: ", yyget_lineno(scanner));
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}
