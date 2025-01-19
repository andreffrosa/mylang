#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

#include "test_utils.h"

static ASTNode* ast;
static SymbolTable* st;

void setUp (void) {
    ast = NULL;
    st = NULL;
}

void tearDown (void) {
    deleteASTNode(&ast);
}

void compileBoolEqChain() { // true == true == true
    ast = newASTCmpEQ(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTCmpEQ(ast, newASTBool(true)).result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, NULL, "true == true && true == true");
}

void compileIntLtLongChain() { // 0 <= 1 == 1 < 2
    ast = newASTCmpLTE(newASTInt(0), newASTInt(1)).result_value;
    ast = newASTCmpEQ(ast, newASTInt(1)).result_value;
    ast = newASTCmpLT(ast, newASTInt(2)).result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, NULL, "0 <= 1 && 1 == 1 && 1 < 2");
}

void compileIntEqChainWithTempVar() { // 1 == 0 + 1 == 1
    ast = newASTCmpEQ(newASTInt(1), newASTAdd(newASTInt(0), newASTInt(1)).result_value).result_value;
    ast = newASTCmpEQ(ast, newASTInt(1)).result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, NULL, "1 == (_tmp_int = 0 + 1) && _tmp_int == 1");
}

void compileTypeEqChain() { // int == bool == void
    ast = newASTCmpEQ(newASTType(AST_TYPE_INT), newASTType(AST_TYPE_BOOL)).result_value;
    ast = newASTCmpEQ(ast, newASTType(AST_TYPE_VOID)).result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, NULL, "_TYPE_INT == _TYPE_BOOL && _TYPE_BOOL == _TYPE_VOID");
}

void compileLtChainWithVar() { // 1 <= n <= 10
    st = newSymbolTableDefault();
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    ast = newASTCmpLTE(newASTInt(1), newASTIDReference("n", st).result_value).result_value;
    ast = newASTCmpLTE(ast, newASTInt(10)).result_value;

    ASSERT_COMPILE_EQUALS(ast, &cSerializer, NULL, "1 <= n && n <= 10");

    deleteSymbolTable(&st);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(compileBoolEqChain);
    RUN_TEST(compileIntLtLongChain);
    RUN_TEST(compileIntEqChainWithTempVar);
    RUN_TEST(compileTypeEqChain);
    RUN_TEST(compileLtChainWithVar);
    return UNITY_END();
}