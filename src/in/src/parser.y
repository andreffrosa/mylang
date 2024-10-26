%define parse.trace
%define parse.error verbose

%define api.pure full
//%locations
%parse-param {yyscan_t scanner} {ParseContext* ctx}
%lex-param {yyscan_t scanner} {ParseContext* ctx}

%code requires {
   typedef void * yyscan_t;

   #include "ast/ast.h"
   typedef struct ParseContext {
      ASTNode** ast;
      SymbolTable* st;
      unsigned int nested_comment_level;
   } ParseContext;

}

%code provides {
   void yyerror(yyscan_t scanner, ParseContext* ctx, const char * s, ...);

   #define YY_DECL \
      int yylex(YYSTYPE* yylval_param, yyscan_t yyscanner, ParseContext* ctx)
   YY_DECL;
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
  bool bval;
  char sval[MAX_ID_SIZE];
  ASTNode* ast_node;
  struct {
      char s1[MAX_ID_SIZE];
      char s2[MAX_ID_SIZE];
      char s3[MAX_ID_SIZE];
  } triple_str;
}

%token <ival> INT_LITERAL
%token <bval> BOOL_LITERAL
%token <sval> ID
%token <sval> TYPE

%token VALUE_OF
%token TYPE_OF

//%right '=' // TODO: rethink
%token '='

// TODO: check priority
%left L_SHIFT R_SHIFT
%left '|' '&' '^' LOGICAL_OR LOGICAL_AND
%left CMP_EQ CMP_NEQ '<' CMP_LTE '>' CMP_GTE
%left '+' '-'
%left '*' '/' '%'

%precedence UMINUS UPLUS SET_POSITIVE SET_NEGATIVE '~' '!'

%token OPEN_ABS CLOSE_ABS

%token VAR

%token PRINT PRINT_VAR

%token <sval> MODIFIER

%token END 0

%type <ast_node> exp stmt stmt_seq pure_exp restr_exp line_stmt scope
%type <sval> type
%type <triple_str> decl

%start program

%destructor { if($$ != NULL) { deleteASTNode(&($$)); } } <ast_node>

%%

program
   : stmt_seq END           { *(ctx->ast) = $1; YYACCEPT; }
   | program error END      { yyerrok; }
   | line_stmt END          { *(ctx->ast) = $1; YYACCEPT; } // If the program is a single statement, it does not require a trailling ';'
   ;

stmt_seq
   : %empty                 { $$ = NULL; }
   | stmt stmt_seq          { $$ = ($2 == NULL) ? $1 : newASTStatementList($1, $2); }
   ;

stmt
   : line_stmt ';'          { $$ = $1; }
   | scope                  { $$ = $1; }
   ;

line_stmt
   : exp                    { $$ = $1; }
   | restr_exp              { $$ = $1; }
   | decl                   { TRY($$, declaration($1.s2, $1.s3, NULL, $1.s1, ctx->st)); }
   | decl '=' exp           { TRY($$, declaration($1.s2, $1.s3,   $3, $1.s1, ctx->st)); }
   | PRINT     '(' exp ')'  { $$ = newASTPrint($3); }
   | PRINT_VAR '('  ID ')'  { TRY($$, handlePrintVar($3, ctx->st)); }
   ;

scope
   : '{' { enterScopeDefault(ctx->st); } stmt_seq '}'
         { leaveScope(ctx->st);
           const ASTNode* stmts = $3 == NULL ? newASTNoOp() : $3;
           $$ = newASTScope(stmts);
         }
   ;

decl
   : type ID                { strncpy($$.s1, "", MAX_ID_SIZE); strncpy($$.s2,    $1, MAX_ID_SIZE); strncpy($$.s3, $2, MAX_ID_SIZE); }
   | VAR  ID                { strncpy($$.s1, "", MAX_ID_SIZE); strncpy($$.s2, "var", MAX_ID_SIZE); strncpy($$.s3, $2, MAX_ID_SIZE); }
   | MODIFIER type ID       { strncpy($$.s1, $1, MAX_ID_SIZE); strncpy($$.s2,    $2, MAX_ID_SIZE); strncpy($$.s3, $3, MAX_ID_SIZE); }
   | MODIFIER VAR  ID       { strncpy($$.s1, $1, MAX_ID_SIZE); strncpy($$.s2, "var", MAX_ID_SIZE); strncpy($$.s3, $3, MAX_ID_SIZE); }
   ;

type
   : TYPE                   { strncpy($$, $1, MAX_ID_SIZE); }
   | ID                     { strncpy($$, $1, MAX_ID_SIZE); }
   ;

exp
   : pure_exp
   | VALUE_OF'('restr_exp')'{ $$ = $3; }
   | TYPE_OF '('restr_exp')'{ $$ = newASTTypeOf($3); } // Built-in function
   ;

restr_exp
   : ID '=' exp             { TRY($$, newASTAssignment($1, $3, ctx->st)); }
   ;

pure_exp
   : INT_LITERAL            { $$ = newASTInt($1); }
   | BOOL_LITERAL           { $$ = newASTBool($1); }
   | TYPE                   { TRY($$, typeFromStr($1)) }
   | ID                     { TRY($$, newASTIDReference($1, ctx->st)); }
   | exp '+' exp            { TRY($$, newASTAdd($1, $3)); }
   | exp '-' exp            { TRY($$, newASTSub($1, $3)); }
   | exp '*' exp            { TRY($$, newASTMul($1, $3)); }
   | exp '/' exp            { TRY($$, newASTDiv($1, $3)); }
   | exp '%' exp            { TRY($$, newASTMod($1, $3)); }
   | '(' exp ')'            { $$ = isCmpExp($2) ? newASTParentheses($2) : $2; }
   | '-' exp %prec UMINUS   { TRY($$, newASTUSub($2)); }
   | '+' exp %prec UPLUS    { TRY($$, newASTUAdd($2)); }
   | exp '&' exp            { TRY($$, newASTBitwiseAnd($1, $3)); }
   | exp '|' exp            { TRY($$, newASTBitwiseOr($1, $3)); }
   | exp '^' exp            { TRY($$, newASTBitwiseXor($1, $3)); }
   | '~' exp                { TRY($$, newASTBitwiseNot($2)); }
   | exp L_SHIFT exp        { TRY($$, newASTLeftShift($1, $3)); }
   | exp R_SHIFT exp        { TRY($$, newASTRightShift($1, $3)); }
   | OPEN_ABS exp CLOSE_ABS { TRY($$, newASTAbs($2)); }
   | SET_POSITIVE exp       { TRY($$, newASTSetPositive($2)); }
   | SET_NEGATIVE exp       { TRY($$, newASTSetNegative($2)); }
   | '!' exp                { TRY($$, newASTLogicalNot($2)); }
   | exp LOGICAL_AND exp    { TRY($$, newASTLogicalAnd($1, $3)); }
   | exp LOGICAL_OR exp     { TRY($$, newASTLogicalOr($1, $3)); }
   | TYPE_OF'('exp')'       { $$ = newASTTypeOf($3); } // Built-in function
   | exp CMP_EQ exp         { TRY($$, newASTCmpEQ($1, $3)); }
   | exp CMP_NEQ exp        { TRY($$, newASTCmpNEQ($1, $3)); }
   | exp '<' exp            { TRY($$, newASTCmpLT($1, $3)); }
   | exp CMP_LTE exp        { TRY($$, newASTCmpLTE($1, $3)); }
   | exp '>' exp            { TRY($$, newASTCmpGT($1, $3)); }
   | exp CMP_GTE exp        { TRY($$, newASTCmpGTE($1, $3)); }
   ;

%%

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void yyerror(yyscan_t scanner, ParseContext* ctx, const char * s, ...) {
  va_list args;
  va_start(args, s);

  fflush(stdout);

  vsyntaxError(yyget_lineno(scanner), s, args);

  va_end(args);
}
#pragma GCC diagnostic pop
