#ifndef _IN_H_
#define _IN_H_

#include <stdio.h>
#include <stdbool.h>

#include "ast/ast.h"

typedef struct InContext InContext;

typedef struct ParseResult {
    bool status;
    ASTNode* ast;
    SymbolTable* st;
} ParseResult;

InContext* inInitWithFile(FILE* file);

InContext* inInitWithString(const char* string);

InContext* inInitWithStdin();

void inDelete(InContext** ctx);

ParseResult inParse(const InContext* ctx);

ParseResult inParseWithSt(const InContext* ctx, SymbolTable* st);

int inLex(const InContext* ctx, void* yylval_param);

unsigned int inGetLineNumber(const InContext* ctx);

#endif
