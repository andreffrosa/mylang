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

%x NESTED_COMMENT

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
"~" |
"<" |
">" |
"{" |
"}" |
"?" |
":"                     { return yytext[0]; }

"(|"                    { return OPEN_ABS; }
"|)"                    { return CLOSE_ABS; }
"<<"                    { return L_SHIFT; }
">>"                    { return R_SHIFT; }
"+|"                    { return SET_POSITIVE; }
"-|"                    { return SET_NEGATIVE; }

"&&"                    { return LOGICAL_AND; }
"||"                    { return LOGICAL_OR; }

"=="                    { return CMP_EQ; }
"!="                    { return CMP_NEQ; }
"<="                    { return CMP_LTE; }
">="                    { return CMP_GTE; }

"+="                    { return ADD_ASS; }
"-="                    { return SUB_ASS; }
"*="                    { return MUL_ASS; }
"/="                    { return DIV_ASS; }
"%="                    { return MOD_ASS; }
"&="                    { return BITWISE_AND_ASS; }
"|="                    { return BITWISE_OR_ASS; }
"^="                    { return BITWISE_XOR_ASS; }
"<<="                   { return LSHIFT_ASS; }
">>="                   { return RSHIFT_ASS; }
"&&="                   { return LOGICAL_AND_ASS; }
"||="                   { return LOGICAL_OR_ASS; }
"++"                    { return INC; }
"--"                    { return DEC; }
"!!"                    { return LOGICAL_TOGGLE; }
"~~"                    { return BITWISE_TOGGLE; }

%{/**** KEYWORDS ****/%}
"var"                   { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return VAR; }

"int"  |
"bool" |
"void" |
"type"                  { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return TYPE; }

"true" |
"false"                 { yylval_param->bval = (yytext[0] == 't'); return BOOL_LITERAL; }

"print"                 { return PRINT; }
"printvar"              { return PRINT_VAR; }
"valueof"               { return VALUE_OF; }
"typeof"                { return TYPE_OF; }

"redef"                 { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return QUALIFIER; }

"if"                    { return IF; }
"then"                  { return THEN; }
"else"                  { return ELSE; }

"while"                 { return WHILE; }
"do"                    { return DO; }
"for"                   { return FOR; }
"loop"                  { return LOOP; }
"break"                 { return BREAK; }
"continue"              { return CONTINUE; }

%{/**** NUMBERS ****/%}
{digit}+                { yylval_param->ival = atoi(yytext); return INT_LITERAL; }
0x{hex_digit}+          { yylval_param->ival = strtol(yytext, NULL, 16); return INT_LITERAL; } // Hexadecimal

%{/**** NAME ****/ %}
[a-zA-Z][a-zA-Z0-9_]*   { strncpy(yylval_param->sval, yytext, MAX_ID_SIZE); return ID; } // Identifier

"//".*/\n?              { /* ignore comments */ }

{whitespace}            { /* ignore whitespace */ }
\\\n                    { /* ignore line continuation */ }
<*>\n                   { /* ignore newline */ }
<*>"/*"                 { if (ctx->nested_comment_level++ == 0) BEGIN(NESTED_COMMENT); }
<NESTED_COMMENT>"*"+"/" { if (--ctx->nested_comment_level == 0) BEGIN(INITIAL); }
<NESTED_COMMENT>.       { /* ignore everything */ }
.                       { yyerror(yyscanner, ctx, "Mystery character: %c\n", *yytext); return YYerror; }
<<EOF>>                 { return END; }
%%