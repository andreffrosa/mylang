#ifndef _OUT_H_
#define _OUT_H_

#include <stdio.h>
#include <stdbool.h>

#include "utils/iostream.h"

#include "ast/ast.h"
#include "ast/symbol.h"

#include "frame.h"

Frame* executeAST(const ASTNode* ast, const SymbolTable* st);

void executeASTStatements(const ASTNode* ast, const SymbolTable* st, Frame* frame);

int evalASTExpression(const ASTNode* node, const SymbolTable* st, Frame* frame);

void outCompileExpression(const ASTNode* ast, const SymbolTable* st, const IOStream* stream);

typedef void (*printFunc)(const IOStream* stream, const char* str, bool printvar);
void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, printFunc print, unsigned int indentation_level);

bool outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream);

bool outCompileToJava(const ASTNode *ast, const SymbolTable *st, const char *file_name, const IOStream *stream);

#endif