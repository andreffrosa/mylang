#ifndef _OUT_H_
#define _OUT_H_

#include <stdio.h>

#include "iostream.h"

#include "ast/ast.h"

int outCompileExpression(ASTNode* ast, IOStream* stream);

int outCompileToC(ASTNode* ast, const char* file_name, IOStream* stream);

int outCompileToJava(ASTNode* ast, const char* file_name, IOStream* stream);

#endif