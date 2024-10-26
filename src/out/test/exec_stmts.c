#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

void setUp (void) {}
void tearDown (void) {}

void declarationAssignmentSetsValue() {
    SymbolTable* st = newSymbolTable(1, 1);
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void assignmentSetsValue() {
    SymbolTable* st = newSymbolTable(1, 1);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false, false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;

    ASTNode* ast = newASTAssignment("n", newASTInt(1), st).result_value;

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarOffset(var);
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void reassignmentChangesValue() {
    SymbolTable* st = newSymbolTable(1,1);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false, false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;
    ASTNode* stmt1 = newASTAssignment("n", newASTInt(1), st).result_value;
    ASTNode* stmt2 = newASTAssignment("n", newASTInt(2), st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarOffset(var);
    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void assignEvalIDGivesSameValue() {
    SymbolTable* st = newSymbolTable(1,1);
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), false, st).result_value;
    ASTNode* stmt2 = newASTIDDeclaration(AST_TYPE_INT, "m", newASTIDReference("n", st).result_value, false, st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "m"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalRestrainedExpressionReturnsValueAndHasSideEffects() {
    SymbolTable* st = newSymbolTable(2, 1);

    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ASTNode* restr_exp = newASTAssignment("n", newASTAdd(newASTIDReference("n", st).result_value, newASTInt(1)).result_value, st).result_value; // valueof(n=n+1)
    ASTNode* stmt2 = newASTIDDeclaration(AST_TYPE_INT, "m", restr_exp, false, st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));
    index = getVarOffset(lookupVar(st, "m"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(declarationAssignmentSetsValue);
    RUN_TEST(assignmentSetsValue);
    RUN_TEST(reassignmentChangesValue);
    RUN_TEST(assignEvalIDGivesSameValue);
    RUN_TEST(evalRestrainedExpressionReturnsValueAndHasSideEffects);
    return UNITY_END();
}