#include <unity.h>

#include "ast.h"

#include "test_utils.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTableDefault();
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
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_VOID);
    deleteASTNode(&ast);
}

void newASTDeclarationCannotHaveVoidType() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_VOID, ID, NULL, false, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_TYPE, res.result_type);
}

void newASTIDReferenceHasDeclaredType() {
    ASTType expected_type = AST_TYPE_INT;
    ASTResult res = newASTIDDeclaration(expected_type, ID, NULL, false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = (ASTNode*) res.result_value;
    //setVarInitialized(decl->child->id);
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ref = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ref, expected_type);
    deleteASTNode(&decl);
    deleteASTNode(&ref);
}

void newASTAssignmentExpressionHasSameTypeAsVar() {
    ASTResult res = defineVar(st, AST_TYPE_TYPE, ID, false);
    TEST_ASSERT_TRUE(isOK(res));

    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* val_node = newASTInt(1);
    res = newASTAssignment(res.result_value, val_node);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void newASTAssignmentOnlyAcceptsExpressions() {
    ASTResult res = defineVar(st, AST_TYPE_TYPE, ID, false);
    TEST_ASSERT_TRUE(isOK(res));

    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = res.result_value;
    ASTNode* val_node = newASTNoOp();
    res = newASTAssignment(id_node, val_node);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void newASTAssignmentHasSameType() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    TEST_ASSERT_TRUE(isOK(res));

    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = res.result_value;
    res = newASTAssignment(id_node, newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;
    ASSERT_EQUAL_VALUE_TYPE(ast, AST_TYPE_INT);
    deleteASTNode(&ast);
}

void newASTDeclarationAssignmentCannotHaveVoidType() {
    ASTNode* val_node = newASTNoOp();
    ASTResult res = newASTIDDeclaration(AST_TYPE_VOID, ID, val_node, false, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_TYPE, res.result_type);
    deleteASTNode(&val_node);
}

void newASTDeclarationAssignmentOnlyAcceptsExpressions() {
    ASTNode* val_node = newASTNoOp();
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, val_node, false, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_INVALID_RIGHT_TYPE, res.result_type);
    deleteASTNode(&val_node);
}

void newASTDeclarationAssignmentExpressionHasSameTypeAsVar() {
    ASTNode* val_node = newASTInt(1);
    ASTResult res = newASTIDDeclaration(AST_TYPE_TYPE, ID, val_node, false, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_DIFFERENT_TYPES, res.result_type);
    deleteASTNode(&val_node);
}

void newASTDeclarationAssignmentIsStatement() {
    ASTNode* val_node = newASTInt(1);
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, val_node, false, st);
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

void validateNewASTType() {
    ASTNode* ast = newASTType(AST_TYPE_INT);

    ASSERT_IS_VALID_AST_NODE(ast, AST_TYPE, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_INT, ast->t);

    deleteASTNode(&ast);
}

void typeofHasTypeType() {
    ASTNode* ast = newASTTypeOf(newASTInt(1));
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, ast->value_type);
    deleteASTNode(&ast);

    ast = newASTTypeOf(newASTBool(true));
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, ast->value_type);
    deleteASTNode(&ast);

    ast = newASTTypeOf(newASTType(AST_TYPE_VOID));
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, ast->value_type);
    deleteASTNode(&ast);

    ast = newASTTypeOf(newASTAdd(newASTInt(1), newASTInt(1)).result_value);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, ast->value_type);
    deleteASTNode(&ast);

    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = res.result_value;
    ast = newASTTypeOf(newASTAssignment(id_node, newASTInt(1)).result_value);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, ast->value_type);
    deleteASTNode(&ast);
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
    RUN_TEST(validateNewASTType);
    RUN_TEST(typeofHasTypeType);
    return UNITY_END();
}
