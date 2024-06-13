%define parse.trace
%define parse.error verbose

%define api.pure full
//%locations
%parse-param {yyscan_t scanner} {ParseContext* ctx}
%lex-param   {yyscan_t scanner} {ParseContext* ctx}

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

#define cpysval3(res, a, b, c) {\
   strncpy(res.s1, a, MAX_ID_SIZE); \
   strncpy(res.s2, b, MAX_ID_SIZE); \
   strncpy(res.s3, c, MAX_ID_SIZE); }

%}

%union {
  int      ival;
  bool     bval;
  char     sval [MAX_ID_SIZE];
  ASTNode* ast_node;
  struct {
     char s1[MAX_ID_SIZE];
     char s2[MAX_ID_SIZE];
     char s3[MAX_ID_SIZE];
  } sval3;
}

%token <ival> INT_LITERAL
%token <bval> BOOL_LITERAL
%token <sval> ID
%token <sval> TYPE VAR QUALIFIER

%token VALUE_OF
%token TYPE_OF

//%right '=' // TODO: rethink
%token '='

%token ADD_ASS SUB_ASS MUL_ASS DIV_ASS MOD_ASS
 BITWISE_AND_ASS BITWISE_OR_ASS BITWISE_XOR_ASS LSHIFT_ASS RSHIFT_ASS
 LOGICAL_AND_ASS LOGICAL_OR_ASS
%token INC DEC LOGICAL_TOGGLE BITWISE_TOGGLE


// TODO: check priority
%right '?' ':'
%left L_SHIFT R_SHIFT
%left '|' '&' '^' LOGICAL_OR LOGICAL_AND
%left CMP_EQ CMP_NEQ '<' CMP_LTE '>' CMP_GTE
%left '+' '-'
%left '*' '/' '%'

%token IF THEN ELSE WHILE DO LOOP FOR BREAK CONTINUE

%precedence UMINUS UPLUS SET_POSITIVE SET_NEGATIVE '~' '!'

%token OPEN_ABS CLOSE_ABS

%token PRINT PRINT_VAR

%token END 0

%type <ast_node> primitive_exp arithmetic_exp logical_exp bitwise_exp cmp_exp cond_exp const_exp assign_exp exp
%type <ast_node> stmt stmt_seq line_stmt scope cond_stmt loop
%type <sval3> decl

%start program

%destructor { if($$ != NULL) { deleteASTNode(&($$)); } } <ast_node>

%%

program
   : stmt_seq END                      { *(ctx->ast) = $1; YYACCEPT; }
   //| program error END                 { yyerrok; }
   // If the program is a single statement, it does not require a trailling ';'
   | line_stmt END                     { *(ctx->ast) = $1; YYACCEPT; } 
   ;

stmt_seq
   : stmt   { $$ = $1; }
   | stmt stmt_seq                     { $$ = newASTStatementList($1, $2); }
   ;

stmt
   : line_stmt ';'                     { $$ = $1; }
   | scope                             { $$ = $1; }
   | cond_stmt                         { $$ = $1; }
   | loop                              { $$ = $1; }
   ;

line_stmt
   : %empty                            { $$ = newASTNoOp(); }
   | exp                               { $$ = $1; }
//[TODO]: Declarations without assignment are disabled while uninitialization verification is not implemented
//   | decl                              { TRY($$, declaration($1.s1, $1.s2, NULL, $1.s3, ctx->st)); }
   | decl '=' exp                      { TRY($$, declaration($1.s1, $1.s2,   $3, $1.s3, ctx->st)); }
   | PRINT'('exp')'                    { $$ = newASTPrint($3); }
   | PRINT_VAR '(' ID ')'              { TRY($$, handlePrintVar($3, ctx->st)); }
   ;

decl
   : TYPE ID                           { cpysval3($$, $1, $2, ""); }
   | VAR  ID                           { cpysval3($$, $1, $2, ""); }
   | QUALIFIER TYPE ID                 { cpysval3($$, $2, $3, $1); }
   | QUALIFIER VAR  ID                 { cpysval3($$, $2, $3, $1); }
   ;

scope
   : '{' '}'                           { $$ = newASTNoOp(); }
   | '{' { enterScopeDefault(ctx->st); }
         stmt_seq '}'                  { leaveScope(ctx->st); $$ = newASTScope($3); }
   ;

cond_stmt
   : IF'('const_exp')'scope                { TRY($$, newASTIf($3, $5)); }
   | IF'('const_exp')'scope ELSE scope     { TRY($$, newASTIfElse($3, $5, $7)); }
   | IF'('const_exp')'scope ELSE cond_stmt { TRY($$, newASTIfElse($3, $5, $7)); }
//   | IF'('const_exp')'THEN line_stmt ';'   { TRY($$, newASTIf($3, $6)); }
//   | IF'('const_exp')' '?' line_stmt ';'   { TRY($$, newASTIf($3, $6)); }
//   | IF'('const_exp')'THEN line_stmt ELSE line_stmt ';' { TRY($$, newASTIfElse($3, $6, $8)); }
//   | IF'('const_exp')' '?' line_stmt  ':' line_stmt ';' { TRY($$, newASTIfElse($3, $6, $8)); }
   ;

loop
   : WHILE '(' exp ')' scope           { TRY($$, newASTWhile($3, $5) ); }
   | LOOP scope                        { TRY($$, newASTWhile(newASTBool(true), $2) ); }
   | DO scope WHILE '(' exp ')'        { TRY($$, newASTDoWhile($2, $5) ); }
   | FOR { enterScopeDefault(ctx->st); }
        '('line_stmt ';' line_stmt ';' line_stmt')' scope
                                       { leaveScope(ctx->st);
                                         TRY($$, newASTFor($4, $6, $8, $10));
                                       }
   | BREAK ';'                         { $$ = newASTBreak(); }
   | CONTINUE ';'                      { $$ = newASTContinue(); }
   ;

exp
   : const_exp                         { $$ = $1; }
   | assign_exp                        { $$ = $1; }
   ;

assign_exp
   : const_exp '=' exp                 { TRY($$, newASTAssignment($1, $3)); }
   | '(' assign_exp ')'                { $$ = $2; }
   | const_exp INC                     { TRY($$, newASTInc($1, false)); }
   | INC const_exp                     { TRY($$, newASTInc($2,  true)); }
   | const_exp DEC                     { TRY($$, newASTDec($1, false)); }
   | DEC const_exp                     { TRY($$, newASTDec($2,  true)); }
   | const_exp LOGICAL_TOGGLE          { TRY($$, newASTLogicalToggle($1, false)); }
   | LOGICAL_TOGGLE const_exp          { TRY($$, newASTLogicalToggle($2,  true)); }
   | const_exp BITWISE_TOGGLE          { TRY($$, newASTBitwiseToggle($1, false)); }
   | BITWISE_TOGGLE const_exp          { TRY($$, newASTBitwiseToggle($2,  true)); }
   | const_exp ADD_ASS exp             { TRY($$, newASTCompoundAssignment(AST_ADD, $1, $3)); }
   | const_exp SUB_ASS exp             { TRY($$, newASTCompoundAssignment(AST_SUB, $1, $3)); }
   | const_exp MUL_ASS exp             { TRY($$, newASTCompoundAssignment(AST_MUL, $1, $3)); }
   | const_exp DIV_ASS exp             { TRY($$, newASTCompoundAssignment(AST_DIV, $1, $3)); }
   | const_exp MOD_ASS exp             { TRY($$, newASTCompoundAssignment(AST_MOD, $1, $3)); }
   | const_exp BITWISE_AND_ASS exp     { TRY($$, newASTCompoundAssignment(AST_BITWISE_AND, $1, $3)); }
   | const_exp BITWISE_OR_ASS exp      { TRY($$, newASTCompoundAssignment(AST_BITWISE_OR, $1,  $3)); }
   | const_exp BITWISE_XOR_ASS exp     { TRY($$, newASTCompoundAssignment(AST_BITWISE_XOR, $1, $3)); }
   | const_exp LSHIFT_ASS exp          { TRY($$, newASTCompoundAssignment(AST_L_SHIFT, $1, $3)); }
   | const_exp RSHIFT_ASS exp          { TRY($$, newASTCompoundAssignment(AST_R_SHIFT, $1, $3)); }
   | const_exp LOGICAL_AND_ASS exp     { TRY($$, newASTCompoundAssignment(AST_LOGICAL_AND, $1, $3)); }
   | const_exp LOGICAL_OR_ASS exp      { TRY($$, newASTCompoundAssignment(AST_LOGICAL_OR, $1, $3)); }
   ;

const_exp
   : primitive_exp                     { $$ = $1; }
   | arithmetic_exp                    { $$ = $1; }
   | bitwise_exp                       { $$ = $1; }
   | logical_exp                       { $$ = $1; }
   | cmp_exp                           { $$ = $1; }
   | cond_exp                          { $$ = $1; }
   ;

primitive_exp
   : INT_LITERAL                       { $$ = newASTInt($1); }
   | BOOL_LITERAL                      { $$ = newASTBool($1); }
   | TYPE                              { TRY($$, typeFromStr($1)) }
   | ID                                { TRY($$, newASTIDReference($1, ctx->st)); }
   | '(' const_exp ')'                 { $$ = requireParentheses($2) ? newASTParentheses($2) : $2; }
   | OPEN_ABS const_exp CLOSE_ABS      { TRY($$, newASTAbs($2)); }
   | VALUE_OF '(' exp ')'              { $$ = $3; }
   | TYPE_OF  '(' exp ')'              { $$ = newASTTypeOf($3); }
   ;

arithmetic_exp
   : const_exp '+' const_exp           { TRY($$, newASTAdd($1, $3)); }
   | const_exp '-' const_exp           { TRY($$, newASTSub($1, $3)); }
   | const_exp '*' const_exp           { TRY($$, newASTMul($1, $3)); }
   | const_exp '/' const_exp           { TRY($$, newASTDiv($1, $3)); }
   | const_exp '%' const_exp           { TRY($$, newASTMod($1, $3)); }
   | '-' const_exp %prec UMINUS        { TRY($$, newASTUSub($2));    }
   | '+' const_exp %prec UPLUS         { TRY($$, newASTUAdd($2));    }
   | SET_POSITIVE const_exp            { TRY($$, newASTSetPositive($2)); }
   | SET_NEGATIVE const_exp            { TRY($$, newASTSetNegative($2)); }
   ;

bitwise_exp
   : const_exp '&' const_exp           { TRY($$, newASTBitwiseAnd($1, $3)); }
   | const_exp '|' const_exp           { TRY($$,  newASTBitwiseOr($1, $3)); }
   | const_exp '^' const_exp           { TRY($$, newASTBitwiseXor($1, $3)); }
   | '~' const_exp                     { TRY($$, newASTBitwiseNot($2));     }
   | const_exp L_SHIFT const_exp       { TRY($$,  newASTLeftShift($1, $3)); }
   | const_exp R_SHIFT const_exp       { TRY($$, newASTRightShift($1, $3)); }
   ;

logical_exp
   : const_exp LOGICAL_AND const_exp   { TRY($$, newASTLogicalAnd($1, $3)); }
   | const_exp LOGICAL_OR const_exp    { TRY($$,  newASTLogicalOr($1, $3)); }
   | '!' const_exp                     { TRY($$, newASTLogicalNot($2)); }
   ;

cmp_exp
   : const_exp CMP_EQ  const_exp       { TRY($$,  newASTCmpEQ($1, $3)); }
   | const_exp CMP_NEQ const_exp       { TRY($$, newASTCmpNEQ($1, $3)); }
   | const_exp   '<'   const_exp       { TRY($$,  newASTCmpLT($1, $3)); }
   | const_exp CMP_LTE const_exp       { TRY($$, newASTCmpLTE($1, $3)); }
   | const_exp   '>'   const_exp       { TRY($$,  newASTCmpGT($1, $3)); }
   | const_exp CMP_GTE const_exp       { TRY($$, newASTCmpGTE($1, $3)); }
   ;

cond_exp
   : const_exp'?'const_exp':'const_exp { TRY($$, newASTTernaryCond($1, $3, $5)); }
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
