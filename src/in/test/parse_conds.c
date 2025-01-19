#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#include "test_utils.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1, 3);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

void parseIfStmtWithoutBody() {
    ASTResult res = newASTIf(newASTBool(true), newASTNoOp());
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    ASSERT_MATCH_AST("if (true) { }", ast, false);
}

void parseIfStmt() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st);
    res = newASTIf(newASTBool(true), newASTScope(res.result_value));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    ASSERT_MATCH_AST("if (true) { int n = 1; }", ast, false);
}

void parseIfElseStmt() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st);
    ASTNode* stmt1 = newASTScope(res.result_value);
    res = newASTIDDeclaration(AST_TYPE_INT, "m", newASTInt(2), false, st);
    ASTNode* stmt2 = newASTScope(res.result_value);
    res = newASTIfElse(newASTBool(true), stmt1, stmt2);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    ASSERT_MATCH_AST("if (true) { int n = 1; } else { int m = 2; }", ast, false);
}

void parseTernaryCondExp() {
    ASTResult res = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;
    ASSERT_MATCH_AST("true ? 1 : 2", ast, false);
}

void parseCondAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    defineVar(st, AST_TYPE_INT, "m", false);

    ASTResult res;
    res = newASTIDReference("n", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = res.result_value;
    res = newASTIDReference("m", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* m_node = res.result_value;

    res = newASTTernaryCond(newASTBool(true), n_node, m_node);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTAssignment(newASTParentheses(res.result_value), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_MATCH_AST("(true ? n : m) = 2", ast, true);
}

void parseNestedCondAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    defineVar(st, AST_TYPE_INT, "m", false);
    defineVar(st, AST_TYPE_INT, "k", false);

    ASTResult res;
    res = newASTIDReference("n", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = res.result_value;
    res = newASTIDReference("m", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* m_node = res.result_value;
    res = newASTIDReference("k", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* k_node = res.result_value;

    res = newASTTernaryCond(newASTBool(true), m_node, k_node);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTTernaryCond(newASTBool(false), n_node, newASTParentheses(res.result_value));
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTAssignment(newASTParentheses(res.result_value), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_MATCH_AST("(false ? n : (true ? m : k)) = 2", ast, true);
}

void parseChainedCondAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    defineVar(st, AST_TYPE_INT, "m", false);
    defineVar(st, AST_TYPE_INT, "k", false);

    ASTResult res;
    res = newASTIDReference("n", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = res.result_value;
    res = newASTIDReference("m", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* m_node = res.result_value;
    res = newASTIDReference("k", st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* k_node = res.result_value;

    res = newASTTernaryCond(newASTBool(true), n_node, m_node);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTAssignment(newASTParentheses(res.result_value), newASTInt(2));
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTAssignment(k_node, res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_MATCH_AST("k = (true ? n : m) = 2", ast, true);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseIfStmtWithoutBody);
    RUN_TEST(parseIfStmt);
    RUN_TEST(parseIfElseStmt);
    RUN_TEST(parseTernaryCondExp);
    RUN_TEST(parseCondAssignment);
    RUN_TEST(parseNestedCondAssignment);
    RUN_TEST(parseChainedCondAssignment);
    return UNITY_END();
}
