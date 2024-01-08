#ifndef _OUT_H_
#define _OUT_H_

#include <stdio.h>

#include "ast/ast.h"

int outCompileExpression(ASTNode* ast, FILE* out_file);

int outCompileToC(ASTNode* ast, FILE* out_file);

int outCompileToJava(ASTNode* ast, FILE* out_file);

#endif