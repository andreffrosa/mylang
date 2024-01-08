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

"(|"        { return OPEN_ABS; }
"|)"        { return CLOSE_ABS; }
"|"         { return BITWISE_OR; }
"&"         { return BITWISE_AND; }
"^"         { return BITWISE_XOR; }
"~"         { return BITWISE_NOT; }
"<<"        { return L_SHIFT; }
">>"        { return R_SHIFT; }
"+|"        { return SET_POSITIVE; }
"-|"        { return SET_NEGATIVE; }

[0-9]+      { yylval_param->ival = atoi(yytext); return NUMBER; }
0x[a-f0-9]+ { yylval_param->ival = strtol(yytext, NULL, 16); return NUMBER; } // Hexadecimal

"//".*\n    { /* ignore comments */ }
\n          { return END; }
[ \t]       { /* ignore whitespace */ }
.           { yyerror(yyscanner, NULL, NULL, "Mystery character %c\n", *yytext); }
<<EOF>>     { return END; }
%%