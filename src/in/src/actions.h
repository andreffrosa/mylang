#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "ast/ast.h"

#define TRY(x) if( (x) == NULL ) { YYABORT; }

#define CTX() *(ctx.st)
#define LINE() yyget_lineno(scanner)

ASTNode* declaration(const char* id, SymbolTable* st, int lineno);

ASTNode* declarationAssignment(const char* id, ASTNode* exp, SymbolTable* st, int lineno);

ASTNode* assignment(const char* id, ASTNode* exp, SymbolTable* st, int lineno);

ASTNode* idReference(const char* id, SymbolTable* st, int lineno);

#endif