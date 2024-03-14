#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "ast/ast.h"

#define TRY(x) if( (x) == NULL ) { YYABORT; }

#define ST() *(ctx.st)
#define LINE() yyget_lineno(scanner)

ASTNode* declaration(const char* type_str, const char* id, SymbolTable* st, int lineno);

ASTNode* declarationAssignment(const char* type_str, const char* id, ASTNode* exp, SymbolTable* st, int lineno);

ASTNode* assignment(const char* id, ASTNode* exp, SymbolTable* st, int lineno);

ASTNode* idReference(const char* id, SymbolTable* st, int lineno);

ASTNode* binaryOp(ASTResult res, ASTNode* left, ASTNode* right, int lineno);

ASTNode* unaryOp(ASTResult res, ASTNode* child, int lineno);


#endif