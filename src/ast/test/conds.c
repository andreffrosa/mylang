#include <unity.h>

#include "ast.h"
#include "test_utils.h"

void setUp(void) {}

void tearDown(void) {}

void conditionMustHaveBoolType() {
    ASTNode* stmt1 = NULL, *stmt2 = NULL;
    ASTResult res;

    res = newASTIf(newASTBool(true), newASTNoOp());
    TEST_ASSERT_TRUE(isOK(res));
    deleteASTNode((ASTNode**)&res.result_value);

    stmt1 = newASTNoOp();
    res = newASTIf(newASTInt(1), stmt1);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&stmt1);

    res = newASTIfElse(newASTBool(true), newASTNoOp(), newASTNoOp());
    TEST_ASSERT_TRUE(isOK(res));
    deleteASTNode((ASTNode**)&res.result_value);

    stmt1 = newASTNoOp();
    stmt2 = newASTNoOp();
    res = newASTIfElse(newASTInt(1), stmt1, stmt2);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&stmt1);
    deleteASTNode(&stmt2);

    res = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    deleteASTNode((ASTNode**)&res.result_value);

    stmt1 = newASTInt(1);
    stmt2 = newASTInt(2);
    res = newASTTernaryCond(newASTInt(1), stmt1, stmt2);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&stmt1);
    deleteASTNode(&stmt2);
}

void ternaryOperandsMustHaveSameType() {
    ASTResult res = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTBool(true));
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode((ASTNode**)&res.result_value);
}

void ternaryOperandsCannotHaveVoidType() {
    ASTResult res = newASTTernaryCond(newASTBool(true), newASTNoOp(), newASTNoOp());
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_TYPE, res.result_type);
    deleteASTNode((ASTNode**)&res.result_value);
}

void ternaryCondHasSameTypeOfItsOperands() {
    ASTResult res = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    TEST_ASSERT_EQUAL_INT(AST_TYPE_INT, ast->value_type);
    deleteASTNode(&ast);

    res = newASTTernaryCond(newASTBool(true), newASTBool(true), newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;
    TEST_ASSERT_EQUAL_INT(AST_TYPE_BOOL, ast->value_type);
    deleteASTNode(&ast);

    res = newASTTernaryCond(newASTBool(true), newASTType(AST_TYPE_INT), newASTType(AST_TYPE_BOOL));
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, ast->value_type);
    deleteASTNode(&ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(conditionMustHaveBoolType);
    RUN_TEST(ternaryOperandsMustHaveSameType);
    RUN_TEST(ternaryOperandsCannotHaveVoidType);
    RUN_TEST(ternaryCondHasSameTypeOfItsOperands);
    return UNITY_END();
}
