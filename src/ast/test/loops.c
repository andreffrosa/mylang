#include <unity.h>

#include "ast.h"
#include "test_utils.h"

void setUp(void) {}

void tearDown(void) {}

void whileLoop() {
    ASTResult res = newASTWhile(newASTBool(true), newASTNoOp());
    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;
    ASSERT_IS_VALID_AST_NODE(ast, AST_WHILE, BINARY_OP, 3);
    deleteASTNode(&ast);
}

void whileLoopConditionMustBeBool() {
    ASTResult res = newASTWhile(newASTInt(1), newASTNoOp());
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LEFT_TYPE);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void doWhileLoop() {
    ASTResult res = newASTDoWhile(newASTNoOp(), newASTBool(true));
    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;
    ASSERT_IS_VALID_AST_NODE(ast, AST_DO_WHILE, BINARY_OP, 3);
    deleteASTNode(&ast);
}

void doWhileLoopConditionMustBeBool() {
    ASTResult res = newASTDoWhile(newASTNoOp(), newASTInt(1));
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_RIGHT_TYPE);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void forLoop() {
    ASTResult res = newASTFor(newASTNoOp(), newASTBool(true), newASTNoOp(), newASTNoOp());
    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;
    ASSERT_IS_VALID_AST_NODE(ast, AST_FOR, UNARY_OP, 9);
    deleteASTNode(&ast);
}

void forLoopConditionMustBeBool() {
    ASTResult res = newASTFor(newASTNoOp(), newASTInt(1), newASTNoOp(), newASTNoOp());
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LEFT_TYPE);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);

    res = newASTIf(newASTBool(false), newASTNoOp());
    ASSERT_IS_OK(res);
    res = newASTFor(newASTNoOp(), res.result_value, newASTNoOp(), newASTNoOp());
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LEFT_TYPE);
    ast = res.result_value;
    deleteASTNode(&ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(whileLoop);
    RUN_TEST(whileLoopConditionMustBeBool);
    RUN_TEST(doWhileLoop);
    RUN_TEST(doWhileLoopConditionMustBeBool);
    RUN_TEST(forLoop);
    RUN_TEST(forLoopConditionMustBeBool);
    return UNITY_END();
}