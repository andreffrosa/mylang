#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

void setUp (void) {}
void tearDown (void) {}

void boolEqChain() {
    ASTNode* ast = newASTCmpEQ(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTCmpEQ(ast, newASTBool(true)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_TRUE(result);
}

void typeEq() {
    ASTNode* ast = newASTCmpEQ(newASTType(AST_TYPE_INT), newASTType(AST_TYPE_BOOL)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void intNeq() {
    ASTNode* ast = newASTCmpNEQ(newASTInt(1), newASTInt(2)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_TRUE(result);
}

void intLtChain() {
    ASTNode* ast = newASTCmpLT(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTCmpLTE(ast, newASTInt(3)).result_value;
    ast = newASTCmpEQ(ast, newASTInt(4)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void intGtChain() {
    ASTNode* ast = newASTCmpGT(newASTInt(4), newASTInt(3)).result_value;
    ast = newASTCmpGTE(ast, newASTInt(2)).result_value;
    ast = newASTCmpEQ(ast, newASTInt(1)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void eqChainHasSingleEval() {
    SymbolTable* st = newSymbolTable(1);

    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(0), st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getSymbolTableSize(st));
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = newASTIDReference("n", st);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTAssignment("n", newASTAdd(((ASTNode*) res.result_value), newASTInt(1)).result_value, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* restrained_exp = res.result_value;
    ASTNode* ast = newASTCmpEQ(newASTInt(1), restrained_exp).result_value;
    ast = newASTCmpEQ(ast, newASTInt(1)).result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, 0)); // Single eval, middle exp is only evaluated once

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

void eqChainShortCircuitsIfFalseOnEval() {
    SymbolTable* st = newSymbolTable(1);

    ASTResult res = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(0), st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getSymbolTableSize(st));
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = newASTIDReference("n", st);
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTAssignment("n", newASTAdd(((ASTNode*) res.result_value), newASTInt(1)).result_value, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* restrained_exp = res.result_value;
    ASTNode* ast = newASTCmpEQ(newASTInt(0), newASTInt(1)).result_value;
    ast = newASTCmpEQ(ast, restrained_exp).result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, 0)); // Shortcircuit, restrained_exp is never evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(boolEqChain);
    RUN_TEST(typeEq);
    RUN_TEST(intNeq);
    RUN_TEST(intLtChain);
    RUN_TEST(intGtChain);
    RUN_TEST(eqChainHasSingleEval);
    RUN_TEST(eqChainShortCircuitsIfFalseOnEval);
    return UNITY_END();
}