#include <unity.h>

#include "ast.h"
#include "test_utils.h"

SymbolTable* st;

void setUp(void) { st = newSymbolTableDefault(); }

void tearDown(void) { deleteSymbolTable(&st); }

#define ID "n"
#define VALUE 1

void idReferenceOfUndefinedVarReturnsErr() {
    ASTResult res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);
}

void idReferenceOfUninitiliazedVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false, false);
    TEST_ASSERT_TRUE(isOK(res));

    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_INIT, res.result_type);
}

void idReferenceOfInitiliazedVarReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, true, false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;

    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*)res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_PTR(var, ast->id);

    deleteASTNode(&ast);
}

void idDeclarationOfVarReturnOk() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    Symbol* var = ast->child->id;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECLARATION, UNARY_OP, 2);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->child->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_FALSE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void idDeclarationOfAlreadyDefineVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false, false);
    TEST_ASSERT_TRUE(isOK(res));

    res = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);
}

void idDeclarationAssignmentOfVarReturnOk() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, newASTInt(VALUE), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*)res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECL_ASSIGN, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(ast->left->id));
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);

    deleteASTNode(&ast);
}

void idDeclarationAssignmentOfAlreadyDefinedVarReturnsErr() {
    defineVar(st, AST_TYPE_INT, ID, false, false);

    ASTNode* ast = newASTInt(VALUE);
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, ast, false, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);

    deleteASTNode(&ast);
}

void assignmentToVarReturnOk() {
    ASTNode* ast = newASTInt(VALUE);
    ASTResult res = newASTAssignment(ID, ast, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);

    deleteASTNode(&ast);
}

void assignmentOfAlreadyDefineVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false, false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* expected_var = res.result_value;

    res = newASTAssignment(ID, newASTInt(VALUE), st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    Symbol* var = ast->left->id;
    TEST_ASSERT_EQUAL_PTR(expected_var, var);
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_ASSIGNMENT, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);
    TEST_ASSERT_TRUE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void newASTStatementListWithNull() {
    ASTNode* stmt = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st).result_value;
    ASTNode* ast = newASTStatementList(stmt, NULL);

    TEST_ASSERT_EQUAL_PTR(stmt, ast);

    deleteASTNode(&ast);
}

void newASTStatementListWithAnotherStatement() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st).result_value;
    ASTNode* stmt2 = newASTAssignment(ID, newASTInt(VALUE), st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + 1);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_PTR(stmt2, ast->right);

    deleteASTNode(&ast);
}

void newASTStatementListWithStatementList() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASTNode* stmt2 = newASTAssignment("n", newASTInt(1), st).result_value;
    ASTNode* stmt3 = newASTIDDeclaration(AST_TYPE_INT, "m", newASTInt(2), false, st).result_value;

    ASTNode* list = newASTStatementList(stmt2, stmt3);
    ASTNode* ast = newASTStatementList(stmt1, list);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + stmt3->size + 2);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_INT(AST_STATEMENT_SEQ, ast->right->node_type);

    deleteASTNode(&ast);
}

extern ASTNode* newASTID(const Symbol* id);

void testEqualASTLeafs() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false, false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* n_var = res.result_value;
    res = defineVar(st, AST_TYPE_INT, "m", false, false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* m_var = res.result_value;

    ASSERT_NOT_EQUAL_AST(newASTInt(1), newASTID(n_var));
    ASSERT_EQUAL_AST(newASTID(n_var), newASTID(n_var));
    ASSERT_NOT_EQUAL_AST(newASTID(n_var), newASTID(m_var));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(idReferenceOfUndefinedVarReturnsErr);
    RUN_TEST(idReferenceOfUninitiliazedVarReturnsErr);
    RUN_TEST(idReferenceOfInitiliazedVarReturnsOk);
    RUN_TEST(idDeclarationOfVarReturnOk);
    RUN_TEST(idDeclarationOfAlreadyDefineVarReturnsErr);
    RUN_TEST(idDeclarationAssignmentOfVarReturnOk);
    RUN_TEST(idDeclarationAssignmentOfAlreadyDefinedVarReturnsErr);
    RUN_TEST(assignmentToVarReturnOk);
    RUN_TEST(assignmentOfAlreadyDefineVarReturnsErr);
    RUN_TEST(newASTStatementListWithNull);
    RUN_TEST(newASTStatementListWithAnotherStatement);
    RUN_TEST(newASTStatementListWithStatementList);
    RUN_TEST(testEqualASTLeafs);
    return UNITY_END();
}
