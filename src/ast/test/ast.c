#include <unity.h>

#include "ast.h"

#include "test_utils.h"

void setUp (void) {}
void tearDown (void) {}

void thereIsNoUnknownOperator() {
    for(int i = 0; i < AST_NODE_TYPES_COUNT; i++) {
        TEST_ASSERT_NOT_EQUAL_INT(UNKNOWN_OP, getNodeOpType(i));
    }
}

void deleteASTNodeSetsVarNull() {
    ASTNode* ast = newASTNumber(1);
    deleteASTNode(&ast);
    TEST_ASSERT_NULL(ast);
}

void validateNewASTNumber() {
    const int n = 1;
    ASTNode* ast = newASTNumber(n);

    ASSERT_IS_VALID_AST_NODE(ast, AST_NUMBER, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_INT(n, ast->n);

    deleteASTNode(&ast);
}

void validateNewUnaryOP() {
    ASTNode* child = newASTNumber(1);
    ASTNode* ast = newASTUnaryOP(AST_USUB, child);

    ASSERT_IS_VALID_AST_NODE(ast, AST_USUB, UNARY_OP, child->size + 1);
    TEST_ASSERT_EQUAL_PTR(child, ast->child);

    deleteASTNode(&ast);
}

void validateNewBinaryOP() {
    ASTNode* left = newASTNumber(1);
    ASTNode* right = newASTNumber(1);
    ASTNode* ast = newASTBinaryOP(AST_ADD, left, right);

    ASSERT_IS_VALID_AST_NODE(ast, AST_ADD, BINARY_OP, left->size + right->size + 1);
    TEST_ASSERT_EQUAL_PTR(left, ast->left);
    TEST_ASSERT_EQUAL_PTR(right, ast->right);

    deleteASTNode(&ast);
}

void testEqualASTLeafs() {
    ASSERT_EQUAL_AST(newASTNumber(1), newASTNumber(1));
    ASSERT_NOT_EQUAL_AST(newASTNumber(1), newASTNumber(2));
}

void testEqualASTUnary() {
    ASSERT_EQUAL_AST(newASTUSub(newASTNumber(1)), newASTUSub(newASTNumber(1)));
    ASSERT_NOT_EQUAL_AST(newASTUSub(newASTNumber(1)), newASTUSub(newASTNumber(2)));
    ASSERT_NOT_EQUAL_AST(newASTUSub(newASTNumber(1)), newASTUAdd(newASTNumber(1)));
}

void testEqualASTBinary() {
    ASTNode* ast1 = newASTAdd(newASTNumber(1), newASTNumber(1));
    ASTNode* ast2 = newASTAdd(newASTNumber(1), newASTNumber(1));
    ASSERT_EQUAL_AST(ast1, ast2);

    ast1 = newASTAdd(newASTNumber(1), newASTNumber(1));
    ast2 = newASTAdd(newASTNumber(1), newASTNumber(2));
    ASSERT_NOT_EQUAL_AST(ast1, ast2);

    ast1 = newASTAdd(newASTNumber(1), newASTNumber(1));
    ast2 = newASTSub(newASTNumber(1), newASTNumber(1));
    ASSERT_NOT_EQUAL_AST(ast1, ast2);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(thereIsNoUnknownOperator);
    RUN_TEST(validateNewASTNumber);
    RUN_TEST(deleteASTNodeSetsVarNull);
    RUN_TEST(validateNewUnaryOP);
    RUN_TEST(validateNewBinaryOP);
    RUN_TEST(testEqualASTLeafs);
    RUN_TEST(testEqualASTUnary);
    RUN_TEST(testEqualASTBinary);
    return UNITY_END();
}
