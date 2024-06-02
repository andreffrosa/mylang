#ifndef _IN_TEST_UTILS_H_
#define _IN_TEST_UTILS_H_

#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#define ASSERT_MATCH_AST_EXP(str, expected_ast) do {\
    InContext* ctx = inInitWithString(str);\
    ParseResult res = inParse(ctx);\
    TEST_ASSERT_TRUE_MESSAGE(res.status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, res.ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&res.ast);\
    deleteSymbolTable(&res.st);\
    inDelete(&ctx);\
} while (0)

#endif