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


void parseIDDeclaration() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASSERT_MATCH_AST("int n", ast, false);
}

void parseIDDeclarationAssignement() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASSERT_MATCH_AST("int n = 1", ast, false);
}

void parseAssignement() {
    defineVar(st, AST_TYPE_INT, "n", false, false);
    ASTNode* ast = newASTAssignment("n", newASTInt(1), st).result_value;
    ASSERT_MATCH_AST("n = 1", ast, true);
}

void parseSingleStatement() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASSERT_MATCH_AST("int n = 1;", ast, false);
}

void parseMultipleStatements() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ASTNode* stmt2 = newASTAssignment("n", newASTInt(1), st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);
    ASSERT_MATCH_AST("int n = 0; n = 1;", ast, false);
}

void parseRestrainedExpression() {
    defineVar(st, AST_TYPE_INT, "n", true, false);

    ASTNode* id = newASTIDReference("n", st).result_value;
    ASTNode* v = newASTAdd(id, newASTInt(1)).result_value;
    ASTNode* ast = newASTAssignment("n", v, st).result_value;
    ASSERT_MATCH_AST("valueof(n = n + 1)", ast, true);
}

void parseDeclarationAssignmentWithTypeInference() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASSERT_MATCH_AST("var n = 1", ast, false);

    ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(true), false, st).result_value;
    ASSERT_MATCH_AST("var z = true", ast, false);
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
    return UNITY_END();
}
