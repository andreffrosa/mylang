#include <unity.h>

#include "ast.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

#define ASSERT_EQUAL_VALUE_TYPE(ast, expected) TEST_ASSERT_EQUAL_INT(expected, ast->value_type)

#define ID "n"

void newASTIntHasTypeInt() {
    ASTNode* ast = newASTInt(1);
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_INT);
    deleteASTNode(&ast);
}

void newASTDeclarationIsStatement() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_VOID);
    deleteASTNode(&ast);
}

void newASTDeclarationCannotHaveVoidType() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_VOID, ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_CHILD_TYPE, res.result_type);
}

void newASTIDReferenceHasDeclaredType() {
    ASTType expected_type = AST_TYPE_INT;
    ASTResult res = newASTIDDeclaration(expected_type, ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = (ASTNode*) res.result_value;
    setVarInitialized(decl->child->id);
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ref = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ref, expected_type);
    deleteASTNode(&decl);
    deleteASTNode(&ref);
}

void newASTAssignmentExpressionHasSameTypeAsVar() {
    insertVar(st, AST_TYPE_TYPE, ID);
    ASTNode* val_node = newASTInt(1);
    ASTResult res = newASTAssignment(ID, val_node, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&val_node);
}

void newASTAssignmentOnlyAcceptsExpressions() {
    insertVar(st, AST_TYPE_INT, ID);
    ASTNode* val_node = newASTNoOp();
    ASTResult res = newASTAssignment(ID, val_node, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&val_node);
}

void newASTAssignmentHasSameType() {
    insertVar(st, AST_TYPE_INT, ID);
    ASTResult res = newASTAssignment(ID, newASTInt(1), st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_INT);
    deleteASTNode(&ast);
}

void newASTDeclarationAssignmentCannotHaveVoidType() {
    ASTNode* val_node = newASTNoOp();
    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_VOID, ID, val_node, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&val_node);
}

void newASTDeclarationAssignmentOnlyAcceptsExpressions() {
    ASTNode* val_node = newASTNoOp();
    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_INT, ID, val_node, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&val_node);
}

void newASTDeclarationAssignmentExpressionHasSameTypeAsVar() {
    ASTNode* val_node = newASTInt(1);
    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_TYPE, ID, val_node, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&val_node);
}

void newASTDeclarationAssignmentIsStatement() {
    ASTNode* val_node = newASTInt(1);
    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_INT, ID, val_node, st);
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_VOID);
    deleteASTNode(&ast);
}

void unaryExpressionsHaveIntType() {
    ASTResult res = newASTUSub(newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_INT);
    deleteASTNode(&ast);
}

void unaryExpressionsOnlyAcceptExpressions() {
    ASTNode* ast = newASTNoOp();
    ASTResult res = newASTUSub(ast);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_CHILD_TYPE, res.result_type);
    deleteASTNode(&ast);
}

void binaryExpressionsHaveIntType() {
    ASTResult res = newASTAdd(newASTInt(1), newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_INT);
    deleteASTNode(&ast);
}

void binaryExpressionsOnlyAcceptExpressions() {
    ASTNode* left = newASTNoOp();
    ASTNode* right = newASTInt(1);
    ASTResult res = newASTAdd(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_LEFT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);

    left = newASTInt(1);
    right = newASTNoOp();
    res = newASTAdd(left, right);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&left);
    deleteASTNode(&right);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(newASTIntHasTypeInt);
    RUN_TEST(newASTIDReferenceHasDeclaredType);
    RUN_TEST(newASTDeclarationCannotHaveVoidType);
    RUN_TEST(newASTDeclarationIsStatement);
    RUN_TEST(newASTAssignmentOnlyAcceptsExpressions);
    RUN_TEST(newASTAssignmentExpressionHasSameTypeAsVar);
    RUN_TEST(newASTAssignmentHasSameType);
    RUN_TEST(newASTDeclarationAssignmentCannotHaveVoidType);
    RUN_TEST(newASTDeclarationAssignmentOnlyAcceptsExpressions);
    RUN_TEST(newASTDeclarationAssignmentExpressionHasSameTypeAsVar);
    RUN_TEST(newASTDeclarationAssignmentIsStatement);
    RUN_TEST(unaryExpressionsHaveIntType);
    RUN_TEST(unaryExpressionsOnlyAcceptExpressions);
    RUN_TEST(binaryExpressionsHaveIntType);
    RUN_TEST(binaryExpressionsOnlyAcceptExpressions);
    return UNITY_END();
}
