#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

#define ASSERT_MATCH_AST(str, expected_ast, clone) do {\
    InContext* ctx = inInitWithString(str);\
    SymbolTable* s = clone ? newSymbolTableClone(st) : newSymbolTableDefault();\
    ParseResult res = inParseWithSt(ctx, s);\
    TEST_ASSERT_TRUE_MESSAGE(res.status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, res.ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&res.ast);\
    deleteSymbolTable(&s);\
    inDelete(&ctx);\
} while (0)

void parseIDDeclaration() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", st).result_value;
    ASSERT_MATCH_AST("int n", ast, false);
}

void parseIDDeclarationAssignement() {
    ASTNode* ast = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(1), st).result_value;
    ASSERT_MATCH_AST("int n = 1", ast, false);
}

void parseAssignement() {
    insertVar(st, AST_TYPE_INT, "n");
    ASTNode* ast = newASTAssignment("n", newASTInt(1), st).result_value;
    ASSERT_MATCH_AST("n = 1", ast, true);
}

void parseSingleStatement() {
    ASTNode* ast = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(1), st).result_value;
    ASSERT_MATCH_AST("int n = 1;", ast, false);
}

void parseMultipleStatements() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", st).result_value;
    ASTNode* stmt2 = newASTAssignment("n", newASTInt(1), st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);
    ASSERT_MATCH_AST("int n; n = 1;", ast, false);
}

void parseRestrainedExpression() {
    Symbol* var = insertVar(st, AST_TYPE_INT, "n");
    setVarInitialized(var);
    ASTNode* id = newASTIDReference("n", st).result_value;
    ASTNode* v = newASTAdd(id, newASTInt(1)).result_value;
    ASTNode* ast = newASTAssignment("n", v, st).result_value;
    ASSERT_MATCH_AST("valueof(n = n + 1)", ast, true);
}

void parseDeclarationAssignmentWithTypeInference() {
    ASTNode* ast = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(1), st).result_value;
    ASSERT_MATCH_AST("var n = 1", ast, false);

    ast = newASTIDDeclarationAssignment(AST_TYPE_BOOL, "z", newASTBool(true), st).result_value;
    ASSERT_MATCH_AST("var z = true", ast, false);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseIDDeclaration);
    RUN_TEST(parseIDDeclarationAssignement);
    RUN_TEST(parseAssignement);
    RUN_TEST(parseSingleStatement);
    RUN_TEST(parseMultipleStatements);
    RUN_TEST(parseRestrainedExpression);
    RUN_TEST(parseDeclarationAssignmentWithTypeInference);
    return UNITY_END();
}
