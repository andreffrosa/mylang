#ifndef _OUT_H_
#define _OUT_H_

#include <stdio.h>

#include "ast/ast.h"

int outCompileExpression(ASTNode* ast, FILE* out_file);

int outCompileToC(ASTNode* ast, const char* file_name, FILE* out_file);

int outCompileToJava(ASTNode* ast, const char* file_name, FILE* out_file);

#endif