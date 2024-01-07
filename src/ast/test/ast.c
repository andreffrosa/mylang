#include <unity.h>

#include <assert.h>

#include "ast.h"

void setUp (void) {}
void tearDown (void) {}

void ASTAddNumbers() {
    ASTNode* add_node = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));

    int result = evalAST(add_node);
    deleteASTNode(add_node);

    TEST_ASSERT_EQUAL_INT(3, result);
}

void ASTAddSub() {
    ASTNode* add_node = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ASTNode* root = newASTBinaryOP(AST_SUB, newASTNumber(5), add_node);

    int result = evalAST(root);
    deleteASTNode(root);

    TEST_ASSERT_EQUAL_INT(2, result);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(ASTAddNumbers);
    RUN_TEST(ASTAddSub);
    return UNITY_END();
}