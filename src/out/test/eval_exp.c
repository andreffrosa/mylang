#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

void setUp (void) {}
void tearDown (void) {}

void testAdditionSequence() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(6, result);
}

void testSubtractionSequence() {
    ASTNode* ast = newASTSub(newASTInt(10), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(5, result);
}

void testAdditionSubtraction() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(newASTInt(5), ast).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(2, result);
}

void testMultiplicationSequence() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(ast, newASTInt(4)).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(24, result);
}

void testMultiplicationAddition() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(7, result);
}

void testDivision() {
    ASTNode* ast = newASTDiv(newASTInt(8), newASTInt(2)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(4, result);
}

void testModulo() {
    ASTNode* ast = newASTMod(newASTInt(10), newASTInt(3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(1, result);
}

void testComplexExpression() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(ast, newASTDiv(newASTInt(10), newASTInt(2)).result_value).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(11, result);
}

void testAbsPositiveValue() {
    ASTNode* ast = newASTAbs(newASTInt(5)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void testAbsNegativeValue() {
    ASTNode* ast = newASTAbs(newASTInt(-3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(3, result);
}

void testSetPositivePositiveValue() {
    ASTNode* ast = newASTSetPositive(newASTInt(5)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void testSetPositiveNegativeValue() {
    ASTNode* ast = newASTSetPositive(newASTInt(-3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(3, result);
}

void testSetNegativePositiveValue() {
    ASTNode* ast = newASTSetNegative(newASTInt(5)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(-5, result);
}

void testSetNegativeNegativeValue() {
    ASTNode* ast = newASTSetNegative(newASTInt(-3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(-3, result);
}

void testBitwiseAND() {
    ASTNode* ast = newASTBitwiseAnd(newASTInt(5), newASTInt(3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(1, result);
}

void testBitwiseOR() {
    ASTNode* ast = newASTBitwiseOr(newASTInt(5), newASTInt(3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(7, result);
}

void testBitwiseXOR() {
    ASTNode* ast = newASTBitwiseXor(newASTInt(5), newASTInt(3)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(6, result);
}

void testBitwiseNot() {
    ASTNode* ast = newASTBitwiseNot(newASTInt(5)).result_value;

    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(-6, result);
}

void testBitwiseShiftLeft() {
    ASTNode* ast = newASTLeftShift(newASTInt(1), newASTInt(2)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(4, result);
}

void testBitwiseShiftRight() {
    ASTNode* ast = newASTRightShift(newASTInt(8), newASTInt(2)).result_value;
    int result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(2, result);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(testAdditionSequence);
    RUN_TEST(testSubtractionSequence);
    RUN_TEST(testAdditionSubtraction);
    RUN_TEST(testMultiplicationSequence);
    RUN_TEST(testMultiplicationAddition);
    RUN_TEST(testDivision);
    RUN_TEST(testModulo);
    RUN_TEST(testComplexExpression);

    RUN_TEST(testAbsPositiveValue);
    RUN_TEST(testAbsNegativeValue);
    RUN_TEST(testSetPositivePositiveValue);
    RUN_TEST(testSetPositiveNegativeValue);
    RUN_TEST(testSetNegativePositiveValue);
    RUN_TEST(testSetNegativeNegativeValue);

    RUN_TEST(testBitwiseAND);
    RUN_TEST(testBitwiseOR);
    RUN_TEST(testBitwiseXOR);
    RUN_TEST(testBitwiseNot);
    RUN_TEST(testBitwiseShiftLeft);
    RUN_TEST(testBitwiseShiftRight);

    return UNITY_END();
}