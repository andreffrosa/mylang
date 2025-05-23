#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#include "test_utils.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1, 1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

/*
void parseIDDeclaration() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASSERT_MATCH_AST("int n", ast, false);
}
*/

void parseIDDeclarationAssignement() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASSERT_MATCH_AST("int n = 1", ast, false);
}

void parseAssignement() {
    defineVar(st, AST_TYPE_INT, "n", false);

    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);

    ASTNode* ast = newASTAssignment(n_node, newASTInt(1)).result_value;
    ASSERT_MATCH_AST("n = 1", ast, true);
}

void parseSingleStatement() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASSERT_MATCH_AST("int n = 1;", ast, false);
}

void parseMultipleStatements() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* stmt2 = newASTAssignment(n_node, newASTInt(1)).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);
    ASSERT_MATCH_AST("int n = 0; n = 1;", ast, false);
}

void parseRestrainedExpression() {
    defineVar(st, AST_TYPE_INT, "n", false);

    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* v = newASTAdd(n_node, newASTInt(1)).result_value;
    n_node = newASTID(res.result_value);
    ASTNode* ast = newASTAssignment(n_node, v).result_value;
    ASSERT_MATCH_AST("valueof(n = n + 1)", ast, true);
}

void parseDeclarationAssignmentWithTypeInference() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASSERT_MATCH_AST("var n = 1", ast, false);

    ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(true), false, st).result_value;
    ASSERT_MATCH_AST("var z = true", ast, false);
}

void parsePrefixInc() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTInc(n_node, true).result_value;
    ASSERT_MATCH_AST("++n", ast, true);
}

void parsePostfixInc() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTInc(n_node, false).result_value;
    ASSERT_MATCH_AST("n++", ast, true);
}

void parsePrefixDec() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTDec(n_node, true).result_value;
    ASSERT_MATCH_AST("--n", ast, true);
}

void parsePostfixDec() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTDec(n_node, false).result_value;
    ASSERT_MATCH_AST("n--", ast, true);
}

void parsePrefixLogicalToggle() {
    defineVar(st, AST_TYPE_BOOL, "z", false);
    ASTResult res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* z_node = newASTID(res.result_value);
    ASTNode* ast = newASTLogicalToggle(z_node, true).result_value;
    ASSERT_MATCH_AST("!!z", ast, true);
}

void parsePostfixLogicalToggle() {
    defineVar(st, AST_TYPE_BOOL, "z", false);
    ASTResult res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* z_node = newASTID(res.result_value);
    ASTNode* ast = newASTLogicalToggle(z_node, false).result_value;
    ASSERT_MATCH_AST("z!!", ast, true);
}

void parsePrefixBitwiseToggle() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTBitwiseToggle(n_node, true).result_value;
    ASSERT_MATCH_AST("~~n", ast, true);
}

void parsePostfixBitwiseToggle() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTBitwiseToggle(n_node, false).result_value;
    ASSERT_MATCH_AST("n~~", ast, true);
}

void parseCompoundAssignmentAdd() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTCompoundAssignment(AST_ADD, n_node, newASTInt(2)).result_value;
    ASSERT_MATCH_AST("n += 2", ast, true);
}

void parseCompoundAssignmentMul() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTResult res = getVarReference(st, "n");
    ASSERT_IS_OK(res);
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* ast = newASTCompoundAssignment(AST_MUL, n_node, newASTInt(2)).result_value;
    ASSERT_MATCH_AST("n *= 2", ast, true);
}

void parseCompoundAssignmentBitwiseAnd() {
    defineVar(st, AST_TYPE_BOOL, "z", false);
    ASTResult res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* z_node = newASTID(res.result_value);
    ASTNode* ast = newASTCompoundAssignment(AST_BITWISE_AND, z_node, newASTBool(true)).result_value;
    ASSERT_MATCH_AST("z &= true", ast, true);
}

void parseCompoundAssignmentLogicalAnd() {
    defineVar(st, AST_TYPE_BOOL, "z", false);
    ASTResult res = getVarReference(st, "z");
    ASSERT_IS_OK(res);
    ASTNode* z_node = newASTID(res.result_value);
    ASTNode* ast = newASTCompoundAssignment(AST_LOGICAL_AND, z_node, newASTBool(true)).result_value;
    ASSERT_MATCH_AST("z &&= true", ast, true);
}

int main() {
    UNITY_BEGIN();
//    RUN_TEST(parseIDDeclaration);
    RUN_TEST(parseIDDeclarationAssignement);
    RUN_TEST(parseAssignement);
    RUN_TEST(parseSingleStatement);
    RUN_TEST(parseMultipleStatements);
    RUN_TEST(parseRestrainedExpression);
    RUN_TEST(parseDeclarationAssignmentWithTypeInference);
    RUN_TEST(parsePrefixInc);
    RUN_TEST(parsePostfixInc);
    RUN_TEST(parsePrefixDec);
    RUN_TEST(parsePostfixDec);
    RUN_TEST(parsePrefixLogicalToggle);
    RUN_TEST(parsePostfixLogicalToggle);
    RUN_TEST(parsePrefixBitwiseToggle);
    RUN_TEST(parsePostfixBitwiseToggle);
    RUN_TEST(parseCompoundAssignmentAdd);
    RUN_TEST(parseCompoundAssignmentMul);
    RUN_TEST(parseCompoundAssignmentBitwiseAnd);
    RUN_TEST(parseCompoundAssignmentLogicalAnd);
    return UNITY_END();
}
