#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

#include "test_utils.h"

SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1, 3);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

void compileIfStmt() {
    ASTNode* stmt = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ASTNode* ast = newASTIf(newASTBool(true), newASTScope(stmt)).result_value;

    const char* str = "if (true) {\n    int n = 0;\n}\n";

    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileSingleLineIfStmt() {
    ASTNode* stmt = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ASTNode* ast = newASTIf(newASTBool(true), stmt).result_value;

    const char* str = "if (true) { int n = 0; }\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileIfElseStmt() {
    enterScope(st, 1);
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    leaveScope(st);
    enterScope(st, 1);
    ASTNode* stmt2 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(-1), false, st).result_value;
    leaveScope(st);

    ASTNode* ast = newASTIfElse(newASTBool(true), newASTScope(stmt1), newASTScope(stmt2)).result_value;

    const char* str = "if (true) {\n    int n = 1;\n} else {\n    int n = -1;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileTernaryCondExp() {
    ASTNode* ast = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(-1)).result_value;

    const char* str = "true ? 1 : -1";
    ASSERT_COMPILE_EXP_EQUALS(ast, str);
}

void compileNestedTernaryCondExpsInsertsParentheses() {
    ASTNode* cond = newASTTernaryCond(newASTBool(true), newASTBool(true), newASTBool(false)).result_value;
    ASTNode* ast = newASTTernaryCond(cond, newASTInt(1), newASTInt(-1)).result_value;

    const char* str = "(true ? true : false) ? 1 : -1";
    ASSERT_COMPILE_EXP_EQUALS(ast, str);

    ASTNode* exp = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(2)).result_value;
    ast = newASTTernaryCond(newASTBool(true), exp, newASTInt(-1)).result_value;

    str = "true ? (true ? 1 : 2) : -1";
    ASSERT_COMPILE_EXP_EQUALS(ast, str);

    exp = newASTTernaryCond(newASTBool(true), newASTInt(-1), newASTInt(-2)).result_value;
    ast = newASTTernaryCond(newASTBool(true), newASTInt(1), exp).result_value;

    str = "true ? 1 : (true ? -1 : -2)";
    ASSERT_COMPILE_EXP_EQUALS(ast, str);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(compileIfStmt);
    RUN_TEST(compileSingleLineIfStmt);
    RUN_TEST(compileIfElseStmt);
    RUN_TEST(compileTernaryCondExp);
    RUN_TEST(compileNestedTernaryCondExpsInsertsParentheses);
    return UNITY_END();
}