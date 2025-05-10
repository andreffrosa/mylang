#include <unity.h>

#include "ast.h"
#include "test_utils.h"

SymbolTable* st;

void setUp(void) { st = newSymbolTableDefault(); }

void tearDown(void) { deleteSymbolTable(&st); }

#define ID "n"
#define VALUE 1

void idReferenceOfUndefinedVarReturnsErr() {
    ASTResult res = getVarReference(st, ID);
    ASSERT_IS_ERR(res, AST_RES_ERR_ID_NOT_DEFINED);
}

/*
void idReferenceOfUninitiliazedVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);

    res = newASTIDReference(ID, st);
    ASSERT_IS_ERR(res, AST_RES_ERR_ID_NOT_INIT);
}
*/

void idReferenceOfInitiliazedVarReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);
    Symbol* var = res.result_value;

    res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* ast = newASTID(res.result_value);

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID, ZEROARY_OP, 1);
    TEST_ASSERT_EQUAL_PTR(var, ast->id);

    deleteASTNode(&ast);
}

void idDeclarationOfVarReturnOk() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st);
    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;
    Symbol* var = ast->child->id;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECLARATION, UNARY_OP, 2);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->child->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    //TEST_ASSERT_FALSE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void idDeclarationOfAlreadyDefinedVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);

    res = newASTIDDeclaration(AST_TYPE_INT, ID, NULL, false, st);
    ASSERT_IS_ERR(res, AST_RES_ERR_ID_ALREADY_DEFINED);
}

void idDeclarationAssignmentOfVarReturnOk() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, newASTInt(VALUE), false, st);
    ASSERT_IS_OK(res);
    ASTNode* ast = (ASTNode*)res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_DECL_ASSIGN, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(ast->left->id));
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);

    deleteASTNode(&ast);
}

void idDeclarationAssignmentOfAlreadyDefinedVarReturnsErr() {
    defineVar(st, AST_TYPE_INT, ID, false);
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, newASTInt(VALUE), false, st);
    ASSERT_IS_ERR(res, AST_RES_ERR_ID_ALREADY_DEFINED);
}

void rvalInDeclarationAssignmentMustNotHaveVoidType() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, newASTNoOp(), false, st);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_RIGHT_TYPE);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void lvalAndRvalInDeclarationAssignmentMustHaveSameType() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, ID, newASTBool(true), false, st);
    ASSERT_IS_ERR(res, AST_RES_ERR_DIFFERENT_TYPES);
    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void assignmentToVarReturnOk() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);
    Symbol* expected_var = res.result_value;

    res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* lval = newASTID(res.result_value);

    res = newASTAssignment(lval, newASTInt(VALUE));
    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    Symbol* var = ast->left->id;
    TEST_ASSERT_EQUAL_PTR(expected_var, var);
    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_ASSIGNMENT, BINARY_OP, 3);
    TEST_ASSERT_EQUAL_INT(AST_ID, ast->left->node_type);
    TEST_ASSERT_EQUAL_STRING(ID, getVarId(var));
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);
    TEST_ASSERT_EQUAL_INT(VALUE, ast->right->n);
    //TEST_ASSERT_TRUE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void rvalInAssignmentMustNotHaveVoidType() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* lval = newASTID(res.result_value);
    ASTNode* rval = newASTNoOp();

    res = newASTAssignment(lval, rval);

    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_RIGHT_TYPE);

    deleteASTNode(&lval);
    deleteASTNode(&rval);
}

void lvalAndRvalInAssignmentMustHaveSameType() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* lval = newASTID(res.result_value);
    ASTNode* rval = newASTBool(true);

    res = newASTAssignment(lval, rval);

    ASSERT_IS_ERR(res, AST_RES_ERR_DIFFERENT_TYPES);

    deleteASTNode(&lval);
    deleteASTNode(&rval);
}

void conditionalAssignmentsReturnsOK() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);
    res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* lval1 = newASTID(res.result_value);

    res = defineVar(st, AST_TYPE_INT, "m", false);
    ASSERT_IS_OK(res);
    res = getVarReference(st, "m");
    ASSERT_IS_OK(res);
    ASTNode* lval2 = newASTID(res.result_value);

    res = newASTTernaryCond(newASTBool(true), lval1, lval2);
    ASSERT_IS_OK(res);
    ASTNode* lval = res.result_value; 

    res = newASTAssignment(lval, newASTInt(VALUE));
    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_ID_ASSIGNMENT, BINARY_OP, 6);
    TEST_ASSERT_EQUAL_INT(AST_INT, ast->right->node_type);

    // TEST_ASSERT_TRUE(isVarInitialized(var));

    deleteASTNode(&ast);
}

void invalidLvalInAssignmentReturnsError() {
    ASTResult res = defineVar(st, AST_TYPE_INT, ID, false);
    ASSERT_IS_OK(res);
    res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* v_node = newASTInt(1);

    res = newASTAdd(n_node, v_node);
    ASSERT_IS_OK(res);
    ASTNode* lval = newASTParentheses(res.result_value);

    res = newASTAssignment(lval, newASTInt(VALUE));
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);
    ASTNode* ast = res.result_value;

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
    ASTResult res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    ASTNode* stmt2 = newASTAssignment(id_node, newASTInt(VALUE)).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + 1);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_PTR(stmt2, ast->right);

    deleteASTNode(&ast);
}

void newASTStatementListWithStatementList() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASTResult res = getVarReference(st, ID);
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    ASTNode* stmt2 = newASTAssignment(id_node, newASTInt(1)).result_value;
    ASTNode* stmt3 = newASTIDDeclaration(AST_TYPE_INT, "m", newASTInt(2), false, st).result_value;

    ASTNode* list = newASTStatementList(stmt2, stmt3);
    ASTNode* ast = newASTStatementList(stmt1, list);

    ASSERT_IS_VALID_AST_NODE(ast, AST_STATEMENT_SEQ, BINARY_OP, stmt1->size + stmt2->size + stmt3->size + 2);
    TEST_ASSERT_EQUAL_PTR(stmt1, ast->left);
    TEST_ASSERT_EQUAL_INT(AST_STATEMENT_SEQ, ast->right->node_type);

    deleteASTNode(&ast);
}

void testEqualASTLeafs() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);
    Symbol* n_var = res.result_value;
    res = defineVar(st, AST_TYPE_INT, "m", false);
    ASSERT_IS_OK(res);
    Symbol* m_var = res.result_value;

    ASSERT_NOT_EQUAL_AST(newASTInt(1), newASTID(n_var));
    ASSERT_EQUAL_AST(newASTID(n_var), newASTID(n_var));
    ASSERT_NOT_EQUAL_AST(newASTID(n_var), newASTID(m_var));
}

void prefixIncOfLvalReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTInc(id_node, true);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_INC, UNARY_OP, 6);

    deleteASTNode(&ast);
}

void prefixIncOfNonLvalReturnsErr() {
    ASTResult res = newASTInc(newASTInt(1), true);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void postfixIncOfLvalReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTInc(id_node, false);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_INC, UNARY_OP, 6);

    deleteASTNode(&ast);
}

void postfixIncOfNonLvalReturnsErr() {
    ASTResult res = newASTInc(newASTInt(1), false);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void prefixIncOfNonIntReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTInc(id_node, true);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_CHILD_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void postfixIncOfNonIntReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTInc(id_node, false);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_CHILD_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void prefixLogicalToggleOfLvalReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTLogicalToggle(id_node, true);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_LOGICAL_TOGGLE, UNARY_OP, 5);

    deleteASTNode(&ast);
}

void prefixLogicalToggleOfNonLvalReturnsErr() {
    ASTResult res = newASTLogicalToggle(newASTBool(true), true);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void postfixLogicalToggleOfLvalReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTLogicalToggle(id_node, false);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_LOGICAL_TOGGLE, UNARY_OP, 5);

    deleteASTNode(&ast);
}

void postfixLogicalToggleOfNonLvalReturnsErr() {
    ASTResult res = newASTLogicalToggle(newASTBool(true), false);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void prefixLogicalToggleOfNonBoolReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTLogicalToggle(id_node, true);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_CHILD_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void postfixLogicalToggleOfNonBoolReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTLogicalToggle(id_node, true);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_CHILD_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void prefixBitwiseToggleOfLvalReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTBitwiseToggle(id_node, true);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_BITWISE_TOGGLE, UNARY_OP, 5);

    deleteASTNode(&ast);
}

void prefixBitwiseToggleOfNonLvalReturnsErr() {
    ASTResult res = newASTBitwiseToggle(newASTBool(true), true);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void postfixBitwiseToggleOfLvalReturnsOk() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTBitwiseToggle(id_node, false);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_BITWISE_TOGGLE, UNARY_OP, 5);

    deleteASTNode(&ast);
}

void postfixBitwiseToggleOfNonLvalReturnsErr() {
    ASTResult res = newASTBitwiseToggle(newASTBool(true), false);
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void bitwiseToggleOfIntReturnsOK() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTBitwiseToggle(id_node, false);

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;

    ASSERT_IS_VALID_AST_NODE(ast, AST_BITWISE_TOGGLE, UNARY_OP, 5);

    deleteASTNode(&ast);
}

void compoundAssignmentOfNonLvalReturnsErr() {
    ASTResult res = newASTCompoundAssignment(AST_ADD, newASTInt(1), newASTInt(2));
    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LVAL);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void compoundAssignmentAddOfIntReturnsOK() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTCompoundAssignment(AST_ADD, id_node, newASTInt(2));

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;
    ASSERT_IS_VALID_AST_NODE(ast, AST_COMPD_ASSIGN, UNARY_OP, 6);

    deleteASTNode(&ast);
}

void compoundAssignmentAddOfNonIntVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTCompoundAssignment(AST_ADD, id_node, newASTInt(1));

    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LEFT_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void compoundAssignmentAddOfNonIntValueReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTCompoundAssignment(AST_ADD, id_node, newASTBool(true));

    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_RIGHT_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void compoundAssignmentLogicalAndOfBoolReturnsOK() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTCompoundAssignment(AST_LOGICAL_AND, id_node, newASTBool(true));

    ASSERT_IS_OK(res);
    ASTNode* ast = res.result_value;
    ASSERT_IS_VALID_AST_NODE(ast, AST_COMPD_ASSIGN, UNARY_OP, 6);

    deleteASTNode(&ast);
}

void compoundAssignmentLogicalAndOfNonBoolVarReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTCompoundAssignment(AST_LOGICAL_AND, id_node, newASTBool(true));

    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_LEFT_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

void compoundAssignmentLogicalAndOfNonBoolValueReturnsErr() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    ASSERT_IS_OK(res);

    res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTCompoundAssignment(AST_LOGICAL_AND, id_node, newASTInt(1));

    ASSERT_IS_ERR(res, AST_RES_ERR_INVALID_RIGHT_TYPE);

    ASTNode* ast = res.result_value;
    deleteASTNode(&ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(idReferenceOfUndefinedVarReturnsErr);
    //RUN_TEST(idReferenceOfUninitiliazedVarReturnsErr);
    RUN_TEST(idReferenceOfInitiliazedVarReturnsOk);
    RUN_TEST(idDeclarationOfVarReturnOk);
    RUN_TEST(idDeclarationOfAlreadyDefinedVarReturnsErr);
    RUN_TEST(idDeclarationAssignmentOfVarReturnOk);
    RUN_TEST(idDeclarationAssignmentOfAlreadyDefinedVarReturnsErr);
    RUN_TEST(rvalInDeclarationAssignmentMustNotHaveVoidType);
    RUN_TEST(lvalAndRvalInDeclarationAssignmentMustHaveSameType);
    RUN_TEST(assignmentToVarReturnOk);
    RUN_TEST(rvalInAssignmentMustNotHaveVoidType);
    RUN_TEST(lvalAndRvalInAssignmentMustHaveSameType);
    RUN_TEST(conditionalAssignmentsReturnsOK);
    RUN_TEST(invalidLvalInAssignmentReturnsError);
    RUN_TEST(newASTStatementListWithNull);
    RUN_TEST(newASTStatementListWithAnotherStatement);
    RUN_TEST(newASTStatementListWithStatementList);
    RUN_TEST(testEqualASTLeafs);
    RUN_TEST(prefixIncOfLvalReturnsOk);
    RUN_TEST(prefixIncOfNonLvalReturnsErr);
    RUN_TEST(postfixIncOfLvalReturnsOk);
    RUN_TEST(postfixIncOfNonLvalReturnsErr);
    RUN_TEST(prefixIncOfNonIntReturnsErr);
    RUN_TEST(postfixIncOfNonIntReturnsErr);
    RUN_TEST(prefixLogicalToggleOfLvalReturnsOk);
    RUN_TEST(prefixLogicalToggleOfNonLvalReturnsErr);
    RUN_TEST(postfixLogicalToggleOfLvalReturnsOk);
    RUN_TEST(postfixLogicalToggleOfNonLvalReturnsErr);
    RUN_TEST(prefixLogicalToggleOfNonBoolReturnsErr);
    RUN_TEST(postfixLogicalToggleOfNonBoolReturnsErr);
    RUN_TEST(prefixBitwiseToggleOfLvalReturnsOk);
    RUN_TEST(prefixBitwiseToggleOfNonLvalReturnsErr);
    RUN_TEST(postfixBitwiseToggleOfLvalReturnsOk);
    RUN_TEST(postfixBitwiseToggleOfNonLvalReturnsErr);
    RUN_TEST(bitwiseToggleOfIntReturnsOK);
    RUN_TEST(compoundAssignmentOfNonLvalReturnsErr);
    RUN_TEST(compoundAssignmentAddOfIntReturnsOK);
    RUN_TEST(compoundAssignmentAddOfNonIntVarReturnsErr);
    RUN_TEST(compoundAssignmentAddOfNonIntValueReturnsErr);
    RUN_TEST(compoundAssignmentLogicalAndOfBoolReturnsOK);
    RUN_TEST(compoundAssignmentLogicalAndOfNonBoolVarReturnsErr);
    RUN_TEST(compoundAssignmentLogicalAndOfNonBoolValueReturnsErr);
    return UNITY_END();
}
