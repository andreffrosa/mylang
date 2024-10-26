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

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseIfStmtWithoutBody);
    RUN_TEST(parseIfStmt);
    RUN_TEST(parseIfElseStmt);
    RUN_TEST(parseTernaryCondExp);
    return UNITY_END();
}
