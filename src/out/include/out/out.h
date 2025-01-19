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

typedef struct OutSerializer {
    void (*parseType)(const IOStream* stream, const ASTType type, const bool in_exp);
    void (*typeOf)(const IOStream* stream, const ASTNode* node, const char* node_str);
    void (*print)(const char* exp_str, const ASTType type, const char* id_str, const IOStream* stream);
    bool (*cond_assign_needs_tmp)();
    bool print_redef_level;
} OutSerializer;

void outCompileAST(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, unsigned int indentation_level);

void compileASTExpression(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os);

void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, unsigned int indentation_level, bool print_new_line);

bool outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream);

bool outCompileToJava(const ASTNode *ast, const SymbolTable *st, const char *file_name, const IOStream *stream);

#endif