%define parse.trace
%define parse.error verbose

%define api.pure full
//%locations
%parse-param {yyscan_t scanner} {ParseContext ctx}
%lex-param {yyscan_t scanner}

%code requires {
  typedef void * yyscan_t;

  #include "parse_ctx.h"
}

%code provides {
  void yyerror(yyscan_t scanner, ParseContext ctx, const char * s, ...);

  /*#define YY_DECL \
       int yylex(YYSTYPE* yylval_param, yyscan_t yyscanner, ASTNode** ast)
   YY_DECL;*/
}

%{
#include <stdio.h>
#include <stdarg.h>

#include "parser.h"
#include "lexer.h"

#include "actions.h"
%}

%union {
  int ival;
  char sval[MAX_ID_SIZE];
  ASTNode* ast_node;
}

%token <ival> NUMBER
%token <sval> ID

//%right '=' // TODO: rethink
%token '='

%left L_SHIFT R_SHIFT
%left BITWISE_OR BITWISE_AND BITWISE_XOR
%left '+' '-'
%left '*' '/' '%'

%precedence UMINUS UPLUS SET_POSITIVE SET_NEGATIVE BITWISE_NOT

%token OPEN_ABS CLOSE_ABS

%token VAR

%token END 0

%type <ast_node> exp stmt stmt_seq

%start program

%%

program: stmt_seq END       { *(ctx.ast) = $1; YYACCEPT; }
   | program error END      { yyerrok; }
   ;

stmt_seq: %empty            { $$ = NULL; }
   | stmt                   { $$ = $1; } // If the program is a single statement, it does not require a trailling ';'
   | stmt ';' stmt_seq      { $$ = newASTStatementList($1, $3); }
   ;

stmt: exp                   { $$ = $1; }
   | VAR ID                 { TRY( $$ = declaration($2, CTX(), LINE()) ); }
   | VAR ID '=' exp         { TRY( $$ = declarationAssignment($2, $4, CTX(), LINE()) ); }
   | ID '=' exp             { TRY( $$ = assignment($1, $3, CTX(), LINE()) ); }
   ;

exp: NUMBER                 { $$ = newASTNumber($1); }
   | ID                     { TRY( $$ = idReference($1, CTX(), LINE()) ); }
   | exp '+' exp            { $$ = newASTAdd($1, $3); }
   | exp '-' exp            { $$ = newASTSub($1, $3); }
   | exp '*' exp            { $$ = newASTMul($1, $3); }
   | exp '/' exp            { $$ = newASTDiv($1, $3); }
   | exp '%' exp            { $$ = newASTMod($1, $3); }
   | '(' exp ')'            { $$ = $2; }
   | '-' exp %prec UMINUS   { $$ = newASTUSub($2); }
   | '+' exp %prec UPLUS    { $$ = newASTUAdd($2); }
   | exp BITWISE_AND exp    { $$ = newASTBitwiseAnd($1, $3); }
   | exp BITWISE_OR exp     { $$ = newASTBitwiseOr($1, $3); }
   | exp BITWISE_XOR exp    { $$ = newASTBitwiseXor($1, $3); }
   | BITWISE_NOT exp        { $$ = newASTBitwiseNot($2); }
   | exp L_SHIFT exp        { $$ = newASTLeftShift($1, $3); }
   | exp R_SHIFT exp        { $$ = newASTRightShift($1, $3); }
   | OPEN_ABS exp CLOSE_ABS { $$ = newASTAbs($2); }
   | SET_POSITIVE exp       { $$ = newASTSetPositive($2); }
   | SET_NEGATIVE exp       { $$ = newASTSetNegative($2); }
   ;

%%

void yyerror(yyscan_t scanner, ParseContext ctx, const char * s, ...) {
  va_list ap;
  va_start(ap, s);

  fflush(stdout);

  fprintf(stderr, "(line %d) PARSE ERROR: ", yyget_lineno(scanner));
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}