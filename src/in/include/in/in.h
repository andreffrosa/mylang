#ifndef _IN_H_
#define _IN_H_

#include <stdio.h>
#include <stdbool.h>

#include "ast/ast.h"

#include "parse_ctx.h"

typedef void* InContext;

InContext inInitWithFile(FILE* file);

InContext inInitWithString(const char* string);

InContext inInitWithStdin();

void inDelete(InContext* in_ctx);

bool inParse(InContext in_ctx, ParseContext parse_ctx);

int inLex(InContext in_ctx, void* yylval_param);

unsigned int inGetLineNumber(InContext in_ctx);

#endif
