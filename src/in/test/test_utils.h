#ifndef _IN_TEST_UTILS_H_
#define _IN_TEST_UTILS_H_

#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#define ASSERT_MATCH_AST_EXP(str, expected_ast) do {\
    InContext ctx = inInitWithString(str);\
    ASTNode* actual_ast = NULL;\
    bool status = inParse(ctx, (ParseContext){&actual_ast, NULL});\
    TEST_ASSERT_TRUE_MESSAGE(status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, actual_ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&actual_ast);\
    inDelete(&ctx);\
} while (0)


#endif