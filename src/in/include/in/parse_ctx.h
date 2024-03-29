#ifndef _IN_PARSE_CTX_H_
#define _IN_PARSE_CTX_H_

#include "ast/ast.h"

typedef struct ParseContext {
    ASTNode** ast;
    SymbolTable** st;
} ParseContext;

#endif
