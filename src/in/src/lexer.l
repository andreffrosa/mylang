%{
#include "ast/ast.h"

#include "parser.h"

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
")" |
"=" |
";"         { return yytext[0]; }

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

%{/**** KEYWORDS ****/%}
"var"       { return VAR; }

"print"     { return PRINT; }
"printvar"  { return PRINT_VAR; }

%{/**** NUMBERS ****/%}
[0-9]+      { yylval_param->ival = atoi(yytext); return NUMBER; }
0x[a-f0-9]+ { yylval_param->ival = strtol(yytext, NULL, 16); return NUMBER; } // Hexadecimal

%{/**** NAME ****/ %}
[a-zA-Z][a-zA-Z0-9_]*  { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return ID; } // Identifier

"//".*\n    { /* ignore comments */ }
\\\n        { /* ignore line continuation */ }
\n          { /* return END; */ }
[ \t]       { /* ignore whitespace */ }

".end\n"    { return END; }

.           { yyerror(yyscanner, (ParseContext){NULL, NULL}, "Mystery character %c\n", *yytext); }
<<EOF>>     { return END; }
%%