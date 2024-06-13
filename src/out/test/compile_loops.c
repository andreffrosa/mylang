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

void compileBreakWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTCmpEQ(copyAST(n_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    body = newASTIf(body, newASTBreak()).result_value;
    body = newASTStatementList(body, newASTInc(copyAST(n_node), false).result_value);
    ast = newASTWhile(cond, newASTScope(body)).result_value;

    const char* str = "while (n < 10) {\n    if (n == 5) { break; }\n    n++;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileContinueWhileLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(newASTInc(copyAST(i_node), false).result_value, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTCmpGTE(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    body = newASTIf(body, newASTContinue()).result_value;
    body = newASTStatementList(body, newASTAssignment(n_node, i_node).result_value);
    ast = newASTStatementList(init, newASTWhile(cond, newASTScope(body)).result_value);

    const char* str = "int i = 0;\nwhile (i++ < 10) {\n    if (i >= 5) { continue; }\n    n = i;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileBreakDoWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* x = newASTCmpEQ(copyAST(n_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTBreak()).result_value;
    x = newASTStatementList(x, newASTInc(copyAST(n_node), false).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTDoWhile(body, cond).result_value;

    const char* str = "do {\n    if (n == 5) { break; }\n    n++;\n} while (n < 10);\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileContinueDoWhileLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(newASTInc(copyAST(i_node), false).result_value, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* x = newASTCmpGTE(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTContinue()).result_value;
    x = newASTStatementList(x, newASTAssignment(n_node, i_node).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTStatementList(init, newASTDoWhile(body, cond).result_value);

    const char* str = "int i = 0;\ndo {\n    if (i >= 5) { continue; }\n    n = i;\n} while (i++ < 10);\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileBreakForLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* x = newASTCmpEQ(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTBreak()).result_value;
    x = newASTStatementList(x, newASTAssignment(n_node, copyAST(i_node)).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTFor(init, cond, update, body).result_value;

    const char* str = "for (int i = 0; i < 10; i++) {\n    if (i == 5) { break; }\n    n = i;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileContinueForLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* x = newASTCmpGTE(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTContinue()).result_value;
    x = newASTStatementList(x, newASTAssignment(n_node, copyAST(i_node)).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTFor(init, cond, update, body).result_value;

    const char* str = "for (int i = 0; i < 10; i++) {\n    if (i >= 5) { continue; }\n    n = i;\n}\n";
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
    RUN_TEST(compileBreakWhileLoop);
    RUN_TEST(compileContinueWhileLoop);
    RUN_TEST(compileBreakDoWhileLoop);
    RUN_TEST(compileContinueDoWhileLoop);
    RUN_TEST(compileBreakForLoop);
    RUN_TEST(compileContinueForLoop);
    return UNITY_END();
}