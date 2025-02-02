#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

#include "test_utils.h"

static SymbolTable* st = NULL;
static ASTNode* ast = NULL;

void setUp (void) {
    st = newSymbolTable(1, 3);
}

void tearDown (void) {
    deleteSymbolTable(&st);
    deleteASTNode(&ast);
}

void compileIfStmt() {
    ASTNode* stmt = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ast = newASTIf(newASTBool(true), newASTScope(stmt)).result_value;

    const char* str = "if (true) {\n    int n = 0;\n}\n";

    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileSingleLineIfStmt() {
    ASTNode* stmt = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ast = newASTIf(newASTBool(true), stmt).result_value;

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

    ast = newASTIfElse(newASTBool(true), newASTScope(stmt1), newASTScope(stmt2)).result_value;

    const char* str = "if (true) {\n    int n = 1;\n} else {\n    int n = -1;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileTernaryCondExp() {
    ast = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(-1)).result_value;

    const char* str = "true ? 1 : -1";
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, str);
}

void compileNestedTernaryCondExpsInsertsParentheses() {
    ASTNode* cond = newASTTernaryCond(newASTBool(true), newASTBool(true), newASTBool(false)).result_value;
    ast = newASTTernaryCond(cond, newASTInt(1), newASTInt(-1)).result_value;

    const char* str = "(true ? true : false) ? 1 : -1";
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, str);
    deleteASTNode(&ast);

    ASTNode* exp = newASTTernaryCond(newASTBool(true), newASTInt(1), newASTInt(2)).result_value;
    ast = newASTTernaryCond(newASTBool(true), exp, newASTInt(-1)).result_value;

    str = "true ? (true ? 1 : 2) : -1";
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, str);
    deleteASTNode(&ast);

    exp = newASTTernaryCond(newASTBool(true), newASTInt(-1), newASTInt(-2)).result_value;
    ast = newASTTernaryCond(newASTBool(true), newASTInt(1), exp).result_value;

    str = "true ? 1 : (true ? -1 : -2)";
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, str);
}

void compileCondAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    defineVar(st, AST_TYPE_INT, "m", false);
    ASTNode* n_node = newASTIDReference("n", st).result_value;
    ASTNode* m_node = newASTIDReference("m", st).result_value;
    ASTNode* cond = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;
    ast = newASTAssignment(newASTParentheses(cond), newASTInt(2)).result_value;

    const char* str = "(true ? (n = 2) : (m = 2));\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);

    str = "_tmp_int = (true ? (n = 2) : (m = 2));\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, str);
}

void compileNestedCondAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    defineVar(st, AST_TYPE_INT, "m", false);
    defineVar(st, AST_TYPE_INT, "k", false);
    ASTNode* n_node = newASTIDReference("n", st).result_value;
    ASTNode* m_node = newASTIDReference("m", st).result_value;
    ASTNode* k_node = newASTIDReference("k", st).result_value;
    ast = newASTTernaryCond(newASTBool(true), m_node, k_node).result_value;
    ast = newASTTernaryCond(newASTBool(false), n_node, newASTParentheses(ast)).result_value;
    ast = newASTAssignment(newASTParentheses(ast), newASTInt(2)).result_value;

    const char* str = "(false ? (n = 2) : (true ? (m = 2) : (k = 2)));\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);

    str = "_tmp_int = (false ? (n = 2) : (true ? (m = 2) : (k = 2)));\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, str);
}

void compileChainedCondAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    defineVar(st, AST_TYPE_INT, "m", false);
    defineVar(st, AST_TYPE_INT, "k", false);
    ASTNode* n_node = newASTIDReference("n", st).result_value;
    ASTNode* m_node = newASTIDReference("m", st).result_value;
    ASTNode* k_node = newASTIDReference("k", st).result_value;
    ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;
    ast = newASTAssignment(newASTParentheses(ast), newASTInt(2)).result_value;
    ast = newASTAssignment(k_node, ast).result_value;

    const char* str = "k = (true ? (n = 2) : (m = 2));\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);

    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, str);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(compileIfStmt);
    RUN_TEST(compileSingleLineIfStmt);
    RUN_TEST(compileIfElseStmt);
    RUN_TEST(compileTernaryCondExp);
    RUN_TEST(compileNestedTernaryCondExpsInsertsParentheses);
    RUN_TEST(compileCondAssignment);
    RUN_TEST(compileNestedCondAssignment);
    RUN_TEST(compileChainedCondAssignment);
    return UNITY_END();
}