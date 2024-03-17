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

%token <ival> INT_LITERAL
%token <sval> ID
%token <sval> TYPE

%token VALUE_OF

//%right '=' // TODO: rethink
%token '='

%left L_SHIFT R_SHIFT
%left BITWISE_OR BITWISE_AND BITWISE_XOR
%left '+' '-'
%left '*' '/' '%'

%precedence UMINUS UPLUS SET_POSITIVE SET_NEGATIVE BITWISE_NOT

%token OPEN_ABS CLOSE_ABS

%token VAR

%token PRINT PRINT_VAR

%token END 0

%type <ast_node> exp stmt stmt_seq pure_exp restr_exp

%start program

%%

program
   : stmt_seq END           { *(ctx.ast) = $1; YYACCEPT; }
   | program error END      { yyerrok; }
   ;

stmt_seq
   : %empty                 { $$ = NULL; }
   | stmt                   { $$ = $1; } // If the program is a single statement, it does not require a trailling ';'
   | stmt ';' stmt_seq      { $$ = newASTStatementList($1, $3); }
   ;

stmt
   : exp                    { $$ = $1; }
   | TYPE ID                { TRY( $$ = declaration($1, $2, ST(), LINE()) ); }
   | TYPE ID '=' exp        { TRY( $$ = declarationAssignment($1, $2, $4, ST(), LINE()) ); }
   | PRINT '(' exp ')'      { $$ = newASTPrint($3); }
   | PRINT_VAR '(' ID ')'   { ASTNode* id; TRY( id = idReference($3, ST(), LINE()) ); $$ = newASTPrintVar(id); }
   | restr_exp              { $$ = $1; }
   ;

exp
   : pure_exp
   | VALUE_OF '(' restr_exp ')' { $$ = $3; }
   ;

restr_exp
   : ID '=' exp             { TRY( $$ = assignment($1, $3, ST(), LINE()) ); }
   ;

pure_exp
   : INT_LITERAL            { $$ = newASTInt($1); }
   | ID                     { TRY( $$ = idReference($1, ST(), LINE()) ); }
   | exp '+' exp            { TRY( $$ = arithmetic(newASTAdd($1, $3), $1, $3, LINE()) ); }
   | exp '-' exp            { TRY( $$ = arithmetic(newASTSub($1, $3), $1, $3, LINE()) ); }
   | exp '*' exp            { TRY( $$ = arithmetic(newASTMul($1, $3), $1, $3, LINE()) ); }
   | exp '/' exp            { TRY( $$ = arithmetic(newASTDiv($1, $3), $1, $3, LINE()) ); }
   | exp '%' exp            { TRY( $$ = arithmetic(newASTMod($1, $3), $1, $3, LINE()) ); }
   | '(' exp ')'            { $$ = $2; }
   | '-' exp %prec UMINUS   { TRY( $$ = uarithmetic(newASTUSub($2), $2, LINE()) ); }
   | '+' exp %prec UPLUS    { TRY( $$ = uarithmetic(newASTUAdd($2), $2, LINE()) ); }
   | exp BITWISE_AND exp    { TRY( $$ = arithmetic(newASTBitwiseAnd($1, $3), $1, $3, LINE()) ); }
   | exp BITWISE_OR exp     { TRY( $$ = arithmetic(newASTBitwiseOr($1, $3), $1, $3, LINE()) ); }
   | exp BITWISE_XOR exp    { TRY( $$ = arithmetic(newASTBitwiseXor($1, $3), $1, $3, LINE()) ); }
   | BITWISE_NOT exp        { TRY( $$ = uarithmetic(newASTBitwiseNot($2), $2, LINE()) ); }
   | exp L_SHIFT exp        { TRY( $$ = arithmetic(newASTLeftShift($1, $3), $1, $3, LINE()) ); }
   | exp R_SHIFT exp        { TRY( $$ = arithmetic(newASTRightShift($1, $3), $1, $3, LINE()) ); }
   | OPEN_ABS exp CLOSE_ABS { TRY( $$ = uarithmetic(newASTAbs($2), $2, LINE()) ); }
   | SET_POSITIVE exp       { TRY( $$ = uarithmetic(newASTSetPositive($2), $2, LINE()) ); }
   | SET_NEGATIVE exp       { TRY( $$ = uarithmetic(newASTSetNegative($2), $2, LINE()) ); }
   ;

%%

void yyerror(yyscan_t scanner, ParseContext ctx, const char * s, ...) {
  va_list ap;
  va_start(ap, s);

  fflush(stdout);

  fprintf(stderr, "(line %d) PARSE ERROR: ", yyget_lineno(scanner));
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");

  va_end(ap);
}
