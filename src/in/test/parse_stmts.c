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
    InContext ctx = inInitWithString(str);\
    ASTNode* actual_ast = NULL;\
    SymbolTable* s = clone ? newSymbolTableClone(st) : NULL;\
    bool status = inParse(ctx, (ParseContext){&actual_ast, &s});\
    TEST_ASSERT_TRUE_MESSAGE(status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, actual_ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&actual_ast);\
    deleteSymbolTable(&s);\
    inDelete(&ctx);\
} while (0)

void parseIDDeclaration() {
    ASTNode* ast = newASTIDDeclaration("n", st).ast;
    ASSERT_MATCH_AST("var n", ast, false);
}

void parseIDDeclarationAssignement() {
    ASTNode* ast = newASTIDDeclarationAssignment("n", newASTNumber(1), st).ast;
    ASSERT_MATCH_AST("var n = 1", ast, false);
}

void parseAssignement() {
    insertVar(st, "n");
    ASTNode* ast = newASTAssignment("n", newASTNumber(1), st).ast;
    ASSERT_MATCH_AST("n = 1", ast, true);
}

void parseSingleStatement() {
    ASTNode* ast = newASTIDDeclarationAssignment("n", newASTNumber(1), st).ast;
    ASSERT_MATCH_AST("var n = 1;", ast, false);
}

void parseMultipleStatements() {
    ASTNode* stmt1 = newASTIDDeclaration("n", st).ast;
    ASTNode* stmt2 = newASTAssignment("n", newASTNumber(1), st).ast;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);
    ASSERT_MATCH_AST("var n; n = 1;", ast, false);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(parseIDDeclaration);
    RUN_TEST(parseIDDeclarationAssignement);
    RUN_TEST(parseAssignement);
    RUN_TEST(parseSingleStatement);
    RUN_TEST(parseMultipleStatements);
    return UNITY_END();
}
