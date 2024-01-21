#include <unity.h>

#include "ast/ast.h"
#include "out/out.h"

static SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

const char* compileStmt(const ASTNode* ast) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTStatements(ast, st, stream, 0);
    IOStreamClose(&stream);
    return ptr;
}

const char* compileExp(const ASTNode* ast) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    outCompileExpression(ast, st, stream);
    IOStreamClose(&stream);
    return ptr;
}

#define ASSERT_COMPILE_STMT_EQUALS(ast, str) {\
    const char* txt = compileStmt(ast);\
    TEST_ASSERT_NOT_NULL(txt);\
    TEST_ASSERT_EQUAL_STRING(str, txt);\
    free((void*)txt);\
    deleteASTNode(&ast);\
}

#define ASSERT_COMPILE_EXP_EQUALS(ast, str) {\
    const char* txt = compileExp(ast);\
    TEST_ASSERT_NOT_NULL(txt);\
    TEST_ASSERT_EQUAL_STRING(str, txt);\
    free((void*)txt);\
    deleteASTNode(&ast);\
}

void testVarDeclaration() {
    ASTNode* ast = newASTIDDeclaration("n", st).ast;
    ASSERT_COMPILE_STMT_EQUALS(ast, "int n;\n");
}

void testVarDeclarationAssignment() {
    ASTNode* ast = newASTIDDeclarationAssignment("n", newASTNumber(100), st).ast;
    ASSERT_COMPILE_STMT_EQUALS(ast, "int n = 100;\n");
}

void testAssignment() {
    Symbol* var = insertVar(st, "n");
    ASTNode* ast = newASTAssignment("n", newASTNumber(100), st).ast;
    ASSERT_COMPILE_STMT_EQUALS(ast, "n = 100;\n");
}

void testIDReference() {
    Symbol* var = insertVar(st, "n");
    setVarInitialized(var);
    ASTNode* id_node = newASTIDReference("n", st).ast;
    ASTNode* ast = newASTAdd(id_node, newASTNumber(1));
    ASSERT_COMPILE_EXP_EQUALS(ast, "n + 1");
}

void testStatementSequence() {
    ASTNode* stmt1 = newASTIDDeclaration("n", st).ast;
    ASTNode* stmt2 = newASTAssignment("n", newASTNumber(100), st).ast;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);
    ASSERT_COMPILE_STMT_EQUALS(ast, "int n;\nn = 100;\n");
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(testVarDeclaration);
    RUN_TEST(testVarDeclarationAssignment);
    RUN_TEST(testAssignment);
    RUN_TEST(testIDReference);
    RUN_TEST(testStatementSequence);
    return UNITY_END();
}