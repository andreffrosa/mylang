#ifndef _IN_TEST_UTILS_H_
#define _IN_TEST_UTILS_H_

#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#define ASSERT_MATCH_AST_EXP(str, expected_ast) do {\
    InContext* _ctx = inInitWithString(str);\
    ParseResult _res = inParse(_ctx);\
    TEST_ASSERT_TRUE_MESSAGE(_res.status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, _res.ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&_res.ast);\
    deleteSymbolTable(&_res.st);\
    inDelete(&_ctx);\
} while (0)

#define ASSERT_MATCH_AST(str, expected_ast, clone) do {\
    InContext* _ctx = inInitWithString(str);\
    SymbolTable* _s = clone ? newSymbolTableClone(st) : newSymbolTableDefault();\
    ParseResult _res = inParseWithSt(_ctx, _s);\
    TEST_ASSERT_TRUE_MESSAGE(_res.status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, _res.ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&_res.ast);\
    deleteSymbolTable(&_s);\
    inDelete(&_ctx);\
} while (0)

#endif