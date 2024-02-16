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
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);

    Symbol* var = insertVar(st,AST_TYPE_INT,  ID);
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_INIT, res.result_type);

    setVarInitialized(var);
    res = newASTIDReference(ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_PTR(var, ast->id);

    deleteASTNode(&ast);
}

void validateNewASTIDDeclaration() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    Symbol* var= ast->child->id;
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECLARATION, UNARY_OP, 2);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->child->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_FALSE(isVarInitialized(var));

    deleteASTNode(&ast);

    res = newASTIDDeclaration(AST_TYPE_INT, ID, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);
}

void validateNewASTIDDeclarationAssignment() {
    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_INT, ID, newASTInt(VALUE), st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = (ASTNode*) res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECL_ASSIGN, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(ast->left->id));
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);

    deleteASTNode(&ast);

    ast = newASTInt(VALUE);
    res = newASTIDDeclarationAssignment(AST_TYPE_INT, ID, ast, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);

    deleteASTNode(&ast);
}

void validateNewASTAssignment() {
    ASTNode* ast = newASTInt(VALUE);
    ASTResult res = newASTAssignment(ID, ast, st);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);

    deleteASTNode(&ast);

    Symbol* s = insertVar(st, AST_TYPE_INT, ID);
    res = newASTAssignment(ID, newASTInt(VALUE), st);
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;
    Symbol* var = ast->left->id;
    TEST_ASSERT_EQUAL_PTR(s, var);
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_ASSIGNMENT, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);
    TEST_ASSERT_TRUE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void newASTStatementListWithNull() {
    ASTNode* stmt = newASTIDDeclaration(AST_TYPE_INT, ID, st).result_value;
    ASTNode* ast = newASTStatementList(stmt, NULL);

    TEST_ASSERT_EQUAL_PTR(stmt, ast);

    deleteASTNode(&ast);
}

void newASTStatementListWithAnotherStatement() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, ID, st).result_value;
    ASTNode* stmt2 = newASTAssignment(ID, newASTInt(VALUE), st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + 1);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_PTR(stmt2, ast->right);

    deleteASTNode(&ast);
}

void newASTStatementListWithStatementList() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", st).result_value;
    ASTNode* stmt2 = newASTAssignment("n", newASTInt(1), st).result_value;
    ASTNode* stmt3 = newASTIDDeclarationAssignment(AST_TYPE_INT, "m", newASTInt(2), st).result_value;

    ASTNode* list = newASTStatementList(stmt2, stmt3);
    ASTNode* ast = newASTStatementList(stmt1, list);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + stmt3->size + 2);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_INT(AST_STATEMENT_SEQ, ast->right->node_type);

    deleteASTNode(&ast);
}

extern ASTNode* newASTID(const Symbol* id);

void testEqualASTLeafs() {
    Symbol* n_var = insertVar(st, AST_TYPE_INT, "n");
    Symbol* m_var = insertVar(st, AST_TYPE_INT, "m");

    ASSERT_NOT_EQUAL_AST(newASTInt(1), newASTID(n_var));
    ASSERT_EQUAL_AST(newASTID(n_var), newASTID(n_var));
    ASSERT_NOT_EQUAL_AST(newASTID(n_var), newASTID(m_var));
}

int main() {
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
