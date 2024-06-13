#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

static SymbolTable* st = NULL;
static ASTNode* ast = NULL;
static ASTNode* n_node = NULL;
static Frame* frame = NULL;
static unsigned int n_index = 0;

#define ITERATION_COUNT 10

void setUp (void) {
    st = newSymbolTable(2, 2);
    defineVar(st, AST_TYPE_INT, "n", false);
    n_index = getVarOffset(lookupVar(st, "n"));
    n_node = newASTIDReference("n", st).result_value;
}

void tearDown (void) {
    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void execAST() {
    assert(ast != NULL);

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);
    executeASTStatements(ast, st, frame);
}

void evalWhileLoopWithFalseConditionExecutesZeroTimes() {
    ASTNode* body = newASTScope(newASTInc(n_node, false).result_value);
    ast = newASTWhile(newASTBool(false), body).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, n_index));
}

void evalWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTScope(newASTInc(copyAST(n_node), false).result_value);
    ast = newASTWhile(cond, body).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT, getFrameValue(frame, n_index));
}

void evalDoWhileLoopWithFalseConditionExecutesOneTime() {
    ASTNode* body = newASTScope(newASTInc(n_node, false).result_value);
    ast = newASTDoWhile(body, newASTBool(false)).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, n_index));
}

void evalDoWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTScope(newASTInc(copyAST(n_node), false).result_value);
    ast = newASTDoWhile(body, cond).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT, getFrameValue(frame, n_index));
}

void evalForLoopWithFalseConditionExecutesZeroTimes() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTBool(false);
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* body = newASTScope(newASTAssignment(n_node, copyAST(i_node)).result_value);
    ast = newASTFor(init, cond, update, body).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, n_index));
}

void evalForLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* body = newASTScope(newASTAssignment(n_node, copyAST(i_node)).result_value);
    ast = newASTFor(init, cond, update, body).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT - 1, getFrameValue(frame, n_index));
}

void evalBreakWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTCmpEQ(copyAST(n_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    body = newASTIf(body, newASTBreak()).result_value;
    body = newASTStatementList(body, newASTInc(copyAST(n_node), false).result_value);
    ast = newASTWhile(cond, newASTScope(body)).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT / 2, getFrameValue(frame, n_index));
}

void evalContinueWhileLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(newASTInc(copyAST(i_node), false).result_value, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* body = newASTCmpGTE(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    body = newASTIf(body, newASTContinue()).result_value;
    body = newASTStatementList(body, newASTAssignment(n_node, i_node).result_value);
    ast = newASTStatementList(init, newASTWhile(cond, newASTScope(body)).result_value);

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT / 2 - 1, getFrameValue(frame, n_index));
}

void evalBreakDoWhileLoop() {
    ASTNode* cond = newASTCmpLT(n_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* x = newASTCmpEQ(copyAST(n_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTBreak()).result_value;
    x = newASTStatementList(x, newASTInc(copyAST(n_node), false).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTDoWhile(body, cond).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT / 2, getFrameValue(frame, n_index));
}

void evalContinueDoWhileLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(newASTInc(copyAST(i_node), false).result_value, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* x = newASTCmpGTE(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTContinue()).result_value;
    x = newASTStatementList(x, newASTAssignment(n_node, i_node).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTStatementList(init, newASTDoWhile(body, cond).result_value);

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT / 2 - 1, getFrameValue(frame, n_index));
}

void evalBreakForLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* x = newASTCmpEQ(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTBreak()).result_value;
    x = newASTStatementList(x, newASTAssignment(n_node, copyAST(i_node)).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTFor(init, cond, update, body).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT / 2 - 1, getFrameValue(frame, n_index));
}

void evalContinueForLoop() {
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st).result_value;
    ASTNode* i_node = newASTIDReference("i", st).result_value;
    ASTNode* cond = newASTCmpLT(i_node, newASTInt(ITERATION_COUNT)).result_value;
    ASTNode* update = newASTInc(copyAST(i_node), false).result_value;
    ASTNode* x = newASTCmpGTE(copyAST(i_node), newASTInt(ITERATION_COUNT / 2)).result_value;
    x = newASTIf(x, newASTContinue()).result_value;
    x = newASTStatementList(x, newASTAssignment(n_node, copyAST(i_node)).result_value);
    ASTNode* body = newASTScope(x);
    ast = newASTFor(init, cond, update, body).result_value;

    execAST();

    TEST_ASSERT_EQUAL_INT(ITERATION_COUNT / 2 - 1, getFrameValue(frame, n_index));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(evalWhileLoopWithFalseConditionExecutesZeroTimes);
    RUN_TEST(evalWhileLoop);
    RUN_TEST(evalDoWhileLoopWithFalseConditionExecutesOneTime);
    RUN_TEST(evalForLoopWithFalseConditionExecutesZeroTimes);
    RUN_TEST(evalForLoop);
    RUN_TEST(evalBreakDoWhileLoop);
    RUN_TEST(evalContinueDoWhileLoop);
    RUN_TEST(evalBreakWhileLoop);
    RUN_TEST(evalContinueWhileLoop);
    RUN_TEST(evalBreakForLoop);
    RUN_TEST(evalContinueForLoop);
    return UNITY_END();
}