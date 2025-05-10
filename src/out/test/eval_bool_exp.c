#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

void setUp (void) {}
void tearDown (void) {}


void testLogicalAndSequence() {
    ASTNode* ast = newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(false)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void logicalAndDoesNotShortCircuitIfTrueOnEval() {
    SymbolTable* st = newSymbolTable(1, 1);

    ASTResult res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTLogicalAnd(newASTBool(true), res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(getFrameValue(frame, 0)); // No shortcircuit, assignment is evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

void logicalAndShortCircuitsIfFalseOnEval() {
    SymbolTable* st = newSymbolTable(1, 1);

    ASTResult res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* restrained_exp = res.result_value;
    res = newASTLogicalAnd(newASTBool(false), restrained_exp);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(getFrameValue(frame, 0)); // Shortcircuit, assignment is not evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

void testLogicalOrSequence() {
    ASTNode* ast = newASTLogicalOr(newASTBool(false), newASTBool(false)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_TRUE(result);
}

void logicalOrDoesNotShortCircuitIfFalseOnEval() {
    SymbolTable* st = newSymbolTable(1, 1);

    ASTResult res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTLogicalOr(newASTBool(false), res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(getFrameValue(frame, 0)); // No shortcircuit, assignment is evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

void logicalOrShortCircuitsIfTrueOnEval() {
    SymbolTable* st = newSymbolTable(1, 1);

    ASTResult res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* restrained_exp = res.result_value;
    res = newASTLogicalOr(newASTBool(true), restrained_exp);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(getFrameValue(frame, 0)); // Shortcircuit, assignment is not evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

void testLogicalNot() {
    ASTNode* ast = newASTLogicalNot(newASTBool(false)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_TRUE(result);

    ast = newASTLogicalNot(newASTBool(true)).result_value;

    result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void testBitwiseAnd() {
    ASTNode* ast = newASTBitwiseAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTBitwiseAnd(ast, newASTBool(false)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void testBitwiseOr() {
    ASTNode* ast = newASTBitwiseOr(newASTBool(false), newASTBool(false)).result_value;
    ast = newASTBitwiseOr(ast, newASTBool(true)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_TRUE(result);
}

void testBitwiseNot() {
    ASTNode* ast = newASTBitwiseNot(newASTBool(false)).result_value;

    bool result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_TRUE(result);

    ast = newASTBitwiseNot(newASTBool(true)).result_value;

    result = evalASTExpression(ast, NULL, NULL);
    deleteASTNode(&ast);

    TEST_ASSERT_FALSE(result);
}

void bitwiseAndNeverShortCircuitsOnEval() {
    SymbolTable* st = newSymbolTable(1, 1);

    ASTResult res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTBitwiseAnd(newASTBool(true), res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(getFrameValue(frame, 0)); // No shortcircuit, assignment is evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);

    st = newSymbolTable(1, 1);

    res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    stmts = res.result_value;
    frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* restrained_exp = res.result_value;
    res = newASTBitwiseAnd(newASTBool(false), restrained_exp);
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;

    result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(getFrameValue(frame, 0)); // No shortcircuit, assignment is evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

void bitwiseOrNeverShortCircuitsOnEval() {
    SymbolTable* st = newSymbolTable(1, 1);

    ASTResult res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmts = res.result_value;
    Frame* frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    res = newASTBitwiseOr(newASTBool(false), res.result_value);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = res.result_value;

    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(getFrameValue(frame, 0)); // No shortcircuit, assignment is evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);

    st = newSymbolTable(1, 1);

    res = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(false), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    stmts = res.result_value;
    frame = newFrame(getMaxOffset(st) + 1);
    executeASTStatements(stmts, st, frame);
    deleteASTNode(&stmts);

    res = getVarReference(st, "z");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* restrained_exp = res.result_value;
    res = newASTBitwiseOr(newASTBool(true), restrained_exp);
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;

    result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(getFrameValue(frame, 0)); // No shortcircuit, assignment is evaluated

    deleteASTNode(&ast);
    deleteFrame(&frame);
    deleteSymbolTable(&st);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testLogicalAndSequence);
    RUN_TEST(logicalAndDoesNotShortCircuitIfTrueOnEval);
    RUN_TEST(logicalAndShortCircuitsIfFalseOnEval);
    RUN_TEST(testLogicalOrSequence);
    RUN_TEST(logicalOrDoesNotShortCircuitIfFalseOnEval);
    RUN_TEST(logicalOrShortCircuitsIfTrueOnEval);
    RUN_TEST(testLogicalNot);
    RUN_TEST(testBitwiseAnd);
    RUN_TEST(testBitwiseOr);
    RUN_TEST(testBitwiseNot);
    RUN_TEST(bitwiseAndNeverShortCircuitsOnEval);
    RUN_TEST(bitwiseOrNeverShortCircuitsOnEval);
    return UNITY_END();
}