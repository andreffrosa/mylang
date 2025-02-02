#ifndef _AST_TEST_UTILS_H_
#define _AST_TEST_UTILS_H_

#include <unity.h>

#include "ast.h"

#define ASSERT_IS_VALID_AST_NODE(ast, node_type_, op_type, size_) do {\
    char str_[200];\
    sprintf(str_, "%s != %s", nodeTypeToStr(node_type_), nodeTypeToStr(ast->node_type));\
    TEST_ASSERT_EQUAL_INT_MESSAGE(node_type_, ast->node_type, str_);\
    TEST_ASSERT_EQUAL_INT(op_type, getNodeOpType(ast->node_type));\
    TEST_ASSERT_EQUAL_INT(size_, ast->size);\
} while(0)

#define ASSERT_EQUAL_AST(ast1, ast2) do {\
    ASTNode *ast1_ = ast1, *ast2_ = ast2;\
    TEST_ASSERT_TRUE(equalAST(ast1_, ast2_));\
    deleteASTNode(&ast1_);\
    deleteASTNode(&ast2_);\
} while (0)

#define ASSERT_NOT_EQUAL_AST(ast1, ast2) do {\
    ASTNode *ast1_ = ast1, *ast2_ = ast2;\
    TEST_ASSERT_FALSE(equalAST(ast1_, ast2_));\
    deleteASTNode(&ast1_);\
    deleteASTNode(&ast2_);\
} while (0)

#define ASSERT_IS_OK(res) TEST_ASSERT_TRUE_MESSAGE(isOK(res), ASTResultTypeToStr(res.result_type))

#define ASSERT_IS_ERR(res, type) do {\
    TEST_ASSERT_TRUE_MESSAGE(isERR(res), ASTResultTypeToStr(res.result_type)); \
    TEST_ASSERT_EQUAL_INT_MESSAGE(type, res.result_type, ASTResultTypeToStr(res.result_type)); \
} while (0)

#endif