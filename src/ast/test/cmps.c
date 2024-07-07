#include <unity.h>

#include "ast.h"

#include "test_utils.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

#define ASSERT_EQUAL_VALUE_TYPE(ast, expected) TEST_ASSERT_EQUAL_INT(expected, ast->value_type)

#define ID "n"

void binaryEqualCmpInts() {
    ASTResult res = newASTCmpEQ(newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryEqualCmpBools() {
    ASTResult res = newASTCmpEQ(newASTBool(true), newASTBool(false));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryEqualCmpMustHaveSameType() {
    ASTNode* left = newASTInt(1);
    ASTNode* right = newASTBool(false);
    ASTResult res = newASTCmpEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);

    left = newASTBool(false);
    right = newASTInt(1);
    res = newASTCmpEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);

    left = newASTInt(1);
    right = newASTType(AST_TYPE_INT);
    res = newASTCmpEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);

    left = newASTBool(false);
    right = newASTType(AST_TYPE_BOOL);
    res = newASTCmpEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void equalCmpIntsCanBeChained() {
    ASTNode* ast = (ASTNode*) newASTCmpEQ(newASTInt(1), newASTInt(2)).result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    ASTResult res = newASTCmpEQ(ast, newASTInt(3));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void equalCmpBoolsCanBeChained() {
    ASTNode* ast = (ASTNode*) newASTCmpEQ(newASTBool(true), newASTBool(true)).result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    ASTResult res = newASTCmpEQ(ast, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void equalCmpBoolsRightMustNotBeCmp() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = (ASTNode*) newASTCmpEQ(newASTBool(true), newASTBool(true)).result_value;
    ASTResult res = newASTCmpEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void binaryNotEqualCmpInts() {
    ASTResult res = newASTCmpNEQ(newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryNotEqualCmpBools() {
    ASTResult res = newASTCmpNEQ(newASTBool(true), newASTBool(false));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryNotEqualCmpMustHaveSameType() {
    ASTNode* left = newASTInt(1);
    ASTNode* right = newASTBool(false);
    ASTResult res = newASTCmpNEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);

    left = newASTBool(false);
    right = newASTInt(1);
    res = newASTCmpNEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void notEqualCmpCannotBeChained() {
    ASTNode* left = (ASTNode*) newASTCmpNEQ(newASTInt(1), newASTInt(2)).result_value;
    ASTNode* right = newASTInt(3);
    ASTResult res = newASTCmpNEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_NON_TRANSITIVE_CHAINING, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);

    left = (ASTNode*) newASTCmpNEQ(newASTBool(false), newASTBool(false)).result_value;
    right = newASTBool(false);
    res = newASTCmpNEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_NON_TRANSITIVE_CHAINING, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void notEqualCmpBoolsRightMustNotBeCmp() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = (ASTNode*) newASTCmpNEQ(newASTBool(true), newASTBool(true)).result_value;
    ASTResult res = newASTCmpEQ(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void binaryLowerThanCmpInts() {
    ASTResult res = newASTCmpLT(newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryLowerThanCmpBoolsReturnsErr() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = newASTBool(true);
    ASTResult res = newASTCmpLT(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void binaryLowerThanOrEqualCmpInts() {
    ASTResult res = newASTCmpLTE(newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryLowerThanOrEqualCmpBoolsReturnsErr() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = newASTBool(true);
    ASTResult res = newASTCmpLTE(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void transitiveLowerThanChain() {
    ASTNode* ast = (ASTNode*) newASTCmpLT(newASTInt(1), newASTInt(1)).result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    ASTResult res = newASTCmpEQ(ast, newASTInt(3));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    res = newASTCmpLTE(ast, newASTInt(4));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryGreaterThanCmpInts() {
    ASTResult res = newASTCmpGT(newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryGreaterThanCmpBoolsReturnsErr() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = newASTBool(true);
    ASTResult res = newASTCmpGT(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void binaryGreaterThanOrEqualCmpInts() {
    ASTResult res = newASTCmpGTE(newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void binaryGreaterThanOrEqualCmpBoolsReturnsErr() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = newASTBool(true);
    ASTResult res = newASTCmpGTE(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void transitiveGreaterThanChain() {
    ASTNode* ast = (ASTNode*) newASTCmpGT(newASTInt(1), newASTInt(1)).result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    ASTResult res = newASTCmpEQ(ast, newASTInt(3));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    res = newASTCmpGTE(ast, newASTInt(4));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void nonTransitiveChainsReturnErr(){
    ASTNode* left = (ASTNode*) newASTCmpLT(newASTInt(1), newASTInt(1)).result_value;
    ASSERT_EQUAL_VALUE_TYPE(left, AST_TYPE_BOOL);
    ASTResult res = newASTCmpEQ(left, newASTInt(3));
    TEST_ASSERT_TRUE(isOK(res));
    left = (ASTNode*) res.result_value;
    ASTNode* right = newASTInt(4);
    res = newASTCmpGT(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_NON_TRANSITIVE_CHAINING, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void inequalLeftBoolAndRightIntReturnsInvalidLeftTypeError() {
    ASTNode* left = newASTBool(true);
    ASTNode* right = newASTInt(1);
    ASTResult res = newASTCmpLTE(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void inequalLeftIntAndRightBoolReturnsInvalidRightTypeError() {
    ASTNode* left = newASTInt(1);
    ASTNode* right = newASTBool(true);
    ASTResult res = newASTCmpLTE(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void transitiveChainStartingWithEq() {
    ASTNode* ast = (ASTNode*) newASTCmpEQ(newASTInt(1), newASTInt(1)).result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    ASTResult res = newASTCmpLT(ast, newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

void inequalCmpTypesReturnsInvalidLeftType() {
    ASTNode* left = (ASTNode*) newASTType(AST_TYPE_INT);
    ASTNode* right = (ASTNode*) newASTType(AST_TYPE_BOOL);
    ASTResult res = newASTCmpLT(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

void parenthesesBreakChaining() {
    ASTNode* cmp = (ASTNode*) newASTCmpEQ(newASTInt(1), newASTInt(1)).result_value;
    ASTResult res = newASTCmpEQ(newASTParentheses(cmp), newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_BOOL);
    deleteASTNode(&ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(binaryEqualCmpInts);
    RUN_TEST(binaryEqualCmpBools);
    RUN_TEST(binaryEqualCmpMustHaveSameType);
    RUN_TEST(equalCmpIntsCanBeChained);
    RUN_TEST(equalCmpBoolsCanBeChained);
    RUN_TEST(equalCmpBoolsRightMustNotBeCmp);
    RUN_TEST(binaryNotEqualCmpInts);
    RUN_TEST(binaryNotEqualCmpBools);
    RUN_TEST(binaryNotEqualCmpMustHaveSameType);
    RUN_TEST(notEqualCmpCannotBeChained);
    RUN_TEST(notEqualCmpBoolsRightMustNotBeCmp);
    RUN_TEST(binaryLowerThanCmpInts);
    RUN_TEST(binaryLowerThanCmpBoolsReturnsErr);
    RUN_TEST(binaryLowerThanOrEqualCmpInts);
    RUN_TEST(binaryLowerThanOrEqualCmpBoolsReturnsErr);
    RUN_TEST(transitiveLowerThanChain);
    RUN_TEST(binaryGreaterThanCmpInts);
    RUN_TEST(binaryGreaterThanCmpBoolsReturnsErr);
    RUN_TEST(binaryGreaterThanOrEqualCmpInts);
    RUN_TEST(binaryGreaterThanOrEqualCmpBoolsReturnsErr);
    RUN_TEST(transitiveGreaterThanChain);
    RUN_TEST(nonTransitiveChainsReturnErr);
    RUN_TEST(inequalLeftBoolAndRightIntReturnsInvalidLeftTypeError);
    RUN_TEST(inequalLeftIntAndRightBoolReturnsInvalidRightTypeError);
    RUN_TEST(transitiveChainStartingWithEq);
    RUN_TEST(inequalCmpTypesReturnsInvalidLeftType);
    RUN_TEST(parenthesesBreakChaining);
    return UNITY_END();
}
