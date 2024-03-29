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

int evalASTExpression(const ASTNode* node, const SymbolTable* st, const Frame* frame);

int outCompileExpression(const ASTNode* ast, const SymbolTable* st, const IOStream* stream);

void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, unsigned int indentation_level);

int outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream);

int outCompileToJava(const ASTNode *ast, const SymbolTable *st, const char *file_name, const IOStream *stream);

#endif