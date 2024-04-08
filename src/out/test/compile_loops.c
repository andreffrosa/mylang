#include <unity.h>

#include "ast/ast.h"
#include "out/out.h"

#include "test_utils.h"

static SymbolTable* st = NULL;
static ASTNode* ast = NULL;
static ASTNode* n_node = NULL;

#define ITERATION_COUNT 10

void setUp (void) {
    st = newSymbolTable(2, 2);
    defineVar(st, AST_TYPE_INT, "n", false);
    n_node = newASTIDReference("n", st).result_value;
}

void tearDown (void) {
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void compileWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTScope(newASTInc(copyAST(n_node), false).result_value);
    ast = newASTWhile(cond, body).result_value;
    const char* str = "while (n < 10) {\n    n++;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileWhileLoopWithEmptyBody() {
    ASTNode* cond = newASTCmpLT(newASTInc(n_node, false).result_value, newASTInt(ITERATION_COUNT)).result_value;
    ast = newASTWhile(cond, newASTNoOp()).result_value;
    const char* str = "while (n++ < 10);\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileDoWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTScope(newASTInc(copyAST(n_node), false).result_value);
    ast = newASTDoWhile(body, cond).result_value;
    const char* str = "do {\n    n++;\n} while (n < 10);\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileDoWhileLoopWithEmptyBody() {
    ASTNode* cond = newASTCmpLT(newASTInc(n_node, false).result_value, newASTInt(ITERATION_COUNT)).result_value;
    ast = newASTDoWhile(newASTNoOp(), cond).result_value;
    const char* str = "do { } while (n++ < 10);\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileForLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* body = newASTScope(newASTAssignment(n_node, copyAST(i_node)).result_value);
    ast = newASTFor(init, cond, update, body).result_value;
    const char* str = "for (int i = 0; i < 10; i++) {\n    n = i;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileForLoopWithEmptyBody() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* body = newASTNoOp();
    ast = newASTFor(init, cond, update, body).result_value;
    const char* str = "for (int i = 0; i < 10; i++);\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(compileWhileLoop);
    RUN_TEST(compileWhileLoopWithEmptyBody);
    RUN_TEST(compileDoWhileLoop);
    RUN_TEST(compileDoWhileLoopWithEmptyBody);
    RUN_TEST(compileForLoop);
    RUN_TEST(compileForLoopWithEmptyBody);
    return UNITY_END();
}