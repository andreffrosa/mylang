#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

static SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

extern const OutSerializer cSerializer;
extern const OutSerializer javaSerializer;

const char* compileExp(const ASTNode* ast, const OutSerializer* os) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTExpression(ast, st, stream, os);
    IOStreamClose(&stream);
    return ptr;
}

#define ASSERT_COMPILE_EQUALS(ast, os, str) {\
    const char* ptr = compileExp(ast, os);\
    TEST_ASSERT_EQUAL_STRING(str, ptr);\
    free((void*) ptr);\
}

void compileTypeOfIntInC() {
    ASTResult res = newASTTypeOf(newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "(1, _TYPE_INT)");

    deleteASTNode(&ast);
}

void compileTypeOfIntInJava() {
    ASTResult res = newASTTypeOf(newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "(1 > 0 ? _Type.INT : _Type.INT)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolInC() {
    ASTResult res = newASTTypeOf(newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "(true, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolInJava() {
    ASTResult res = newASTTypeOf(newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "(true ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

void compileTypeOfIntExpressionInC() {
    ASTResult res = newASTTypeOf(newASTAdd(newASTInt(1), newASTInt(1)).result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "(1 + 1, _TYPE_INT)");

    deleteASTNode(&ast);
}

void compileTypeOfIntExpressionInJava() {
    ASTResult res = newASTTypeOf(newASTAdd(newASTInt(1), newASTInt(1)).result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "(1 + 1 > 0 ? _Type.INT : _Type.INT)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolExpressionInC() {
    ASTResult res = newASTTypeOf(newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "(true && true, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolExpressionInJava() {
    ASTResult res = newASTTypeOf(newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "(true && true ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofTypeInC() {
    ASTResult res = newASTTypeOf(newASTType(AST_TYPE_INT));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "(_TYPE_INT, _TYPE_TYPE)");

    deleteASTNode(&ast);
}

void compileTypeofTypeInJava() {
    ASTResult res = newASTTypeOf(newASTType(AST_TYPE_INT));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "(_Type.INT == _Type.VOID ? _Type.TYPE : _Type.TYPE)");

    deleteASTNode(&ast);
}

void compileTypeofIDInC() {
    Symbol* var = insertVar(st, AST_TYPE_BOOL, "z");
    setVarInitialized(var);
    ASTResult res = newASTIDReference("z", st);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTTypeOf(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "(z, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofIDInJava() {
    Symbol* var = insertVar(st, AST_TYPE_BOOL, "z");
    setVarInitialized(var);
    ASTResult res = newASTIDReference("z", st);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTTypeOf(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "(z ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofValueofRestrainedExpInC() {
    insertVar(st, AST_TYPE_BOOL, "z");
    ASTResult res = newASTAssignment("z", newASTBool(true), st);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTTypeOf(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, "((z = true), _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofValueofRestrainedExpInJava() {
    insertVar(st, AST_TYPE_BOOL, "z");
    ASTResult res = newASTAssignment("z", newASTBool(true), st);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTTypeOf(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    ASSERT_COMPILE_EQUALS(ast, &javaSerializer, "((z = true) ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(compileTypeOfIntInC);
    RUN_TEST(compileTypeOfIntInJava);
    RUN_TEST(compileTypeOfBoolInC);
    RUN_TEST(compileTypeOfBoolInJava);
    RUN_TEST(compileTypeofTypeInC);
    RUN_TEST(compileTypeofTypeInJava);
    RUN_TEST(compileTypeofIDInC);
    RUN_TEST(compileTypeofIDInJava);
    RUN_TEST(compileTypeofValueofRestrainedExpInC);
    RUN_TEST(compileTypeofValueofRestrainedExpInJava);
    RUN_TEST(compileTypeOfIntExpressionInC);
    RUN_TEST(compileTypeOfIntExpressionInJava);
    RUN_TEST(compileTypeOfBoolExpressionInC);
    RUN_TEST(compileTypeOfBoolExpressionInJava);
    return UNITY_END();
}