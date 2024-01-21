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

#define ID "n"
#define VALUE 1

void validateNewASTIDReference() {
    ASTResult res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(ASTResult_ERR_ID_NOT_DEFINED, res.type);

    Symbol* var = insertVar(st, ID);
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(ASTResult_ERR_ID_NOT_INIT, res.type);

    setVarInitialized(var);
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.ast;
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_PTR(var, ast->id);

    deleteASTNode(&ast);
}

void validateNewASTIDDeclaration() {
    ASTResult res = newASTIDDeclaration(ID, st);
    TEST_ASSERT_TRUE(isOK(res));

    ASTNode* ast = res.ast;
    Symbol* var= ast->child->id;
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECLARATION, UNARY_OP, 2);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->child->type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_FALSE(isVarInitialized(var));

    deleteASTNode(&ast);

    res = newASTIDDeclaration(ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(ASTResult_ERR_ID_ALREADY_DEFINED, res.type);
}

void validateNewASTIDDeclarationAssignment() {
    ASTResult res = newASTIDDeclarationAssignment(ID, newASTNumber(VALUE), st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.ast;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECL_ASSIGN, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(ast->left->id));
    TEST_ASSERT_EQUAL_INT(AST_NUMBER, ast->right->type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);

    deleteASTNode(&ast);

    ast = newASTNumber(VALUE);
    res = newASTIDDeclarationAssignment(ID, ast, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(ASTResult_ERR_ID_ALREADY_DEFINED, res.type);

    deleteASTNode(&ast);
}

void validateNewASTAssignment() {
    ASTNode* ast = newASTNumber(VALUE);
    ASTResult res = newASTAssignment(ID, ast, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(ASTResult_ERR_ID_NOT_DEFINED, res.type);

    deleteASTNode(&ast);

    Symbol* s = insertVar(st, ID);
    res = newASTAssignment(ID, newASTNumber(VALUE), st);
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.ast;
    Symbol* var = ast->left->id;
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_ASSIGNMENT, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_EQUAL_INT(AST_NUMBER, ast->right->type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);
    TEST_ASSERT_TRUE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void newASTStatementListWithNull() {
    ASTNode* stmt = newASTIDDeclaration(ID, st).ast;
    ASTNode* ast = newASTStatementList(stmt, NULL);

    TEST_ASSERT_EQUAL_PTR(stmt, ast);

    deleteASTNode(&ast);
}

void newASTStatementListWithAnotherStatement() {
    ASTNode* stmt1 = newASTIDDeclaration(ID, st).ast;
    ASTNode* stmt2 = newASTAssignment(ID, newASTNumber(VALUE), st).ast;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + 1);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_PTR(stmt2, ast->right);

    deleteASTNode(&ast);
}

void newASTStatementListWithStatementList() {
    ASTNode* stmt1 = newASTIDDeclaration("n", st).ast;
    ASTNode* stmt2 = newASTAssignment("n", newASTNumber(1), st).ast;
    ASTNode* stmt3 = newASTIDDeclarationAssignment("m", newASTNumber(2), st).ast;

    ASTNode* list = newASTStatementList(stmt2, stmt3);
    ASTNode* ast = newASTStatementList(stmt1, list);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + stmt3->size + 2);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_INT(AST_STATEMENT_SEQ, ast->right->type);

    deleteASTNode(&ast);
}

extern ASTNode* newASTID(const Symbol* id);

void testEqualASTLeafs() {
    Symbol* n_var = insertVar(st, "n");
    Symbol* m_var = insertVar(st, "m");

    ASSERT_NOT_EQUAL_AST(newASTNumber(1), newASTID(n_var));
    ASSERT_EQUAL_AST(newASTID(n_var), newASTID(n_var));
    ASSERT_NOT_EQUAL_AST(newASTID(n_var), newASTID(m_var));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(validateNewASTIDReference);
    RUN_TEST(validateNewASTIDDeclaration);
    RUN_TEST(validateNewASTIDDeclarationAssignment);
    RUN_TEST(validateNewASTAssignment);
    RUN_TEST(newASTStatementListWithNull);
    RUN_TEST(newASTStatementListWithAnotherStatement);
    RUN_TEST(newASTStatementListWithStatementList);
    RUN_TEST(testEqualASTLeafs);
    return UNITY_END();
}
