#ifndef _IN_H_
#define _IN_H_

#include <stdio.h>
#include <stdbool.h>

#include "ast/ast.h"

typedef void* ParseContext;

ParseContext inInitWithFile(FILE* file);

ParseContext inInitWithString(const char* string);

ParseContext inInitWithStdin();

void inDelete(ParseContext* ctx);

bool inParse(ParseContext ctx, ASTNode** ast);

int inLex(ParseContext ctx, void* yylval_param);

unsigned int inGetLineNumber(ParseContext ctx);

#endif
