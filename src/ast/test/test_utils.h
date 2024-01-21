#ifndef _AST_TEST_UTILS_H_
#define _AST_TEST_UTILS_H_

#include <unity.h>

#include "ast.h"

#define ASSERT_IS_VALID_AST_NODE(ast, node_type, op_type, size_) do {\
    TEST_ASSERT_EQUAL_INT(node_type, ast->type);\
    TEST_ASSERT_EQUAL_INT(op_type, getNodeOpType(ast->type));\
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


#endif