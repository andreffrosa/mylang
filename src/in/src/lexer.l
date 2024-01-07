%{
#include "ast/ast.h"
#include "parser.h"

void yyerror(yyscan_t scanner, struct ASTNode** ast, const char * s, ...);
%}

%option 8bit noyywrap noinput nounput
%option yylineno
%option warn nodefault
%option reentrant bison-bridge
/* %option bison-locations */

%%
"+" |
"-" |
"*" |
"/" |
"%" |
"(" |
")"         { return yytext[0]; }

[0-9]+      { yylval_param->ival = atoi(yytext); return NUMBER; }

"//".*\n    { /* ignore comments */ }
\n          { return END; }
[ \t]       { /* ignore whitespace */ }
.           { yyerror(yyscanner, NULL, NULL, "Mystery character %c\n", *yytext); }
<<EOF>>     { return END; }
%%