#include <unity.h>

#include <assert.h>

#include "ast.h"

void setUp (void) {}
void tearDown (void) {}

void testAdditionSequence() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(6, result);
}

void testSubtractionSequence() {
    ASTNode* ast = newASTBinaryOP(AST_SUB, newASTNumber(10), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(5, result);
}

void testAdditionSubtraction() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(5), ast);

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(2, result);
}

void testMultiplicationSequence() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, ast, newASTNumber(4));

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(24, result);
}

void testMultiplicationAddition() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(7, result);
}

void testDivision() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(8), newASTNumber(2));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(4, result);
}

void testModulo() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(10), newASTNumber(3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(1, result);
}

void testComplexExpression() {
    ASTNode* ast =  newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, ast, newASTBinaryOP(AST_DIV, newASTNumber(10), newASTNumber(2)));

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(11, result);
}

void testAbsPositiveValue() {
    ASTNode* ast = newASTUnaryOP(AST_ABS, newASTNumber(5));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void testAbsNegativeValue() {
    ASTNode* ast = newASTUnaryOP(AST_ABS, newASTNumber(-3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(3, result);
}

void testSetPositivePositiveValue() {
    ASTNode* ast = newASTUnaryOP(AST_SET_POSITIVE, newASTNumber(5));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void testSetPositiveNegativeValue() {
    ASTNode* ast = newASTUnaryOP(AST_SET_POSITIVE, newASTNumber(-3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(3, result);
}

void testSetNegativePositiveValue() {
    ASTNode* ast = newASTUnaryOP(AST_SET_NEGATIVE, newASTNumber(5));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(-5, result);
}

void testSetNegativeNegativeValue() {
    ASTNode* ast = newASTUnaryOP(AST_SET_NEGATIVE, newASTNumber(-3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(-3, result);
}

void testBitwiseAND() {
    ASTNode* ast = newASTBinaryOP(AST_BITWISE_AND, newASTNumber(5), newASTNumber(3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(1, result);
}

void testBitwiseOR() {
    ASTNode* ast = newASTBinaryOP(AST_BITWISE_OR, newASTNumber(5), newASTNumber(3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(7, result);
}

void testBitwiseXOR() {
    ASTNode* ast = newASTBinaryOP(AST_BITWISE_XOR, newASTNumber(5), newASTNumber(3));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(6, result);
}

void testBitwiseNot() {
    ASTNode* ast = newASTUnaryOP(AST_BITWISE_NOT, newASTNumber(5));

    int result = evalAST(ast);
    deleteASTNode(&ast);

    TEST_ASSERT_EQUAL_INT(-6, result);
}

void testBitwiseShiftLeft() {
    ASTNode* ast = newASTBinaryOP(AST_L_SHIFT, newASTNumber(1), newASTNumber(2));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(4, result);
}

void testBitwiseShiftRight() {
    ASTNode* ast = newASTBinaryOP(AST_R_SHIFT, newASTNumber(8), newASTNumber(2));
    int result = evalAST(ast);
    deleteASTNode(&ast);
    TEST_ASSERT_EQUAL_INT(2, result);
}

int main(int argc, char** argv) {
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