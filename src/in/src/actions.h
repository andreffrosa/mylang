#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "ast/ast.h"

#define LINE() yyget_lineno(scanner)

#define TRY(v, action) if( (v = handleErrors(action, LINE())) == NULL ) { YYABORT; }

void syntaxError(int lineno, const char* s, ...);
void vsyntaxError(int lineno, const char* s, va_list args);

void semanticError(int lineno, const char* s, ...);

ASTNode* handleErrors(ASTResult res, int lineno);

ASTResult declaration(const char* type_str, const char* id, ASTNode* exp, const char* modifier, SymbolTable* st);

ASTResult handlePrintVar(const char* id, SymbolTable* st);

ASTResult parseType(const char* type_str);

ASTResult parseIDReference(SymbolTable* st, const char* id);

#endif