%{
#include "ast/ast.h"

#include "parser.h"

%}

%option 8bit noyywrap noinput nounput
%option yylineno
%option warn nodefault
%option reentrant bison-bridge
/* %option bison-locations */

digit           [0-9]
hex_digit       [a-f0-9]
whitespace      [ \t\r]

%%
"+" |
"-" |
"*" |
"/" |
"%" |
"(" |
")" |
"=" |
";" |
"!" |
"|" |
"&" |
"^" |
"~"                     { return yytext[0]; }

"(|"                    { return OPEN_ABS; }
"|)"                    { return CLOSE_ABS; }
"<<"                    { return L_SHIFT; }
">>"                    { return R_SHIFT; }
"+|"                    { return SET_POSITIVE; }
"-|"                    { return SET_NEGATIVE; }

"&&"                    { return LOGICAL_AND; }
"||"                    { return LOGICAL_OR; }

%{/**** KEYWORDS ****/%}
"var"                   { return VAR; }

"int"  |
"bool"                  { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return TYPE; }

"true" |
"false"                 { yylval_param->bval = (yytext[0] == 't'); return BOOL_LITERAL; }

"print"                 { return PRINT; }
"printvar"              { return PRINT_VAR; }
"valueof"               { return VALUE_OF; }

%{/**** NUMBERS ****/%}
{digit}+                { yylval_param->ival = atoi(yytext); return INT_LITERAL; }
0x{hex_digit}+          { yylval_param->ival = strtol(yytext, NULL, 16); return INT_LITERAL; } // Hexadecimal

%{/**** NAME ****/ %}
[a-zA-Z][a-zA-Z0-9_]*   { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return ID; } // Identifier

"//".*/\n?              { /* ignore comments */ }
{whitespace}            { /* ignore whitespace */ }
\\\n                    { /* ignore line continuation */ }
\n                      { /*return END;*/ }

.                       { yyerror(yyscanner, (ParseContext){NULL, NULL}, "Mystery character %c\n", *yytext); }
<<EOF>>                 { return END; }
%%