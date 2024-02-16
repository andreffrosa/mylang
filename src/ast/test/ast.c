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
    ASTNode* ast = newASTInt(1);
    deleteASTNode(&ast);
    TEST_ASSERT_NULL(ast);
}

void validateNewASTInt() {
    const int n = 1;
    ASTNode* ast = newASTInt(n);

    ASSERT_IS_VALID_AST_NODE(ast, AST_INT, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_INT(n, ast->n);

    deleteASTNode(&ast);
}

void validateNewUnaryOP() {
    ASTNode* child = newASTInt(1);
    ASTResult res = newASTUSub(child);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_USUB, UNARY_OP, child->size + 1);
    TEST_ASSERT_EQUAL_PTR(child, ast->child);

    deleteASTNode(&ast);
}

void validateNewBinaryOP() {
    ASTNode* left = newASTInt(1);
    ASTNode* right = newASTInt(1);
    ASTResult res = newASTAdd(left, right);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ADD, BINARY_OP, left->size + right->size + 1);
    TEST_ASSERT_EQUAL_PTR(left, ast->left);
    TEST_ASSERT_EQUAL_PTR(right, ast->right);
    TEST_ASSERT_EQUAL_INT(left->size + right->size + 1, ast->size);

    deleteASTNode(&ast);
}

void testEqualASTLeafs() {
    ASSERT_EQUAL_AST(newASTInt(1), newASTInt(1));
    ASSERT_NOT_EQUAL_AST(newASTInt(1), newASTInt(2));
}

void testEqualASTUnary() {
    ASSERT_EQUAL_AST(newASTUSub(newASTInt(1)).result_value, newASTUSub(newASTInt(1)).result_value);
    ASSERT_NOT_EQUAL_AST(newASTUSub(newASTInt(1)).result_value, newASTUSub(newASTInt(2)).result_value);
    ASSERT_NOT_EQUAL_AST(newASTUSub(newASTInt(1)).result_value, newASTUAdd(newASTInt(1)).result_value);
}

void testEqualASTBinary() {
    ASTNode* ast1 = (ASTNode*) newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ASTNode* ast2 = (ASTNode*) newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ASSERT_EQUAL_AST(ast1, ast2);

    ast1 = (ASTNode*) newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast2 = (ASTNode*) newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ASSERT_NOT_EQUAL_AST(ast1, ast2);

    ast1 = (ASTNode*) newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast2 = (ASTNode*) newASTSub(newASTInt(1), newASTInt(1)).result_value;
    ASSERT_NOT_EQUAL_AST(ast1, ast2);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(thereIsNoUnknownOperator);
    RUN_TEST(validateNewASTInt);
    RUN_TEST(deleteASTNodeSetsVarNull);
    RUN_TEST(validateNewUnaryOP);
    RUN_TEST(validateNewBinaryOP);
    RUN_TEST(testEqualASTLeafs);
    RUN_TEST(testEqualASTUnary);
    RUN_TEST(testEqualASTBinary);
    return UNITY_END();
}
