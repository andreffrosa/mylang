#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

#include "test_utils.h"

static SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1, 1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

void compileTypeOfIntInC() {
    ASTNode* ast = newASTTypeOf(newASTInt(1));

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(1, _TYPE_INT)");

    deleteASTNode(&ast);
}

void compileTypeOfIntInJava() {
    ASTNode* ast = newASTTypeOf(newASTInt(1));

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "(1 > 0 ? _Type.INT : _Type.INT)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolInC() {
    ASTNode* ast = newASTTypeOf(newASTBool(true));

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolInJava() {
    ASTNode* ast = newASTTypeOf(newASTBool(true));

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "(true ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

void compileTypeOfIntExpressionInC() {
    ASTNode* ast = newASTTypeOf(newASTAdd(newASTInt(1), newASTInt(1)).result_value);

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(1 + 1, _TYPE_INT)");

    deleteASTNode(&ast);
}

void compileTypeOfIntExpressionInJava() {
    ASTNode* ast = newASTTypeOf(newASTAdd(newASTInt(1), newASTInt(1)).result_value);

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "(1 + 1 > 0 ? _Type.INT : _Type.INT)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolExpressionInC() {
    ASTNode* ast = newASTTypeOf(newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value);

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true && true, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeOfBoolExpressionInJava() {
    ASTNode* ast = newASTTypeOf(newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value);

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "(true && true ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofTypeInC() {
    ASTNode* ast = newASTTypeOf(newASTType(AST_TYPE_INT));

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(_TYPE_INT, _TYPE_TYPE)");

    deleteASTNode(&ast);
}

void compileTypeofTypeInJava() {
    ASTNode* ast = newASTTypeOf(newASTType(AST_TYPE_INT));

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "(_Type.INT == _Type.VOID ? _Type.TYPE : _Type.TYPE)");

    deleteASTNode(&ast);
}

void compileTypeofIDInC() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = newASTTypeOf(newASTID(res.result_value));

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(z, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofIDInJava() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = newASTTypeOf(newASTID(res.result_value));

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "(z ? _Type.BOOL : _Type.BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofValueofRestrainedExpInC() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);

    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = newASTTypeOf(res.result_value);

    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(z = true, _TYPE_BOOL)");

    deleteASTNode(&ast);
}

void compileTypeofValueofRestrainedExpInJava() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);

    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = newASTTypeOf(res.result_value);

    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "((z = true) ? _Type.BOOL : _Type.BOOL)");

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