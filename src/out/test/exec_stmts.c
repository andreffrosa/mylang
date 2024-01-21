#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

void setUp (void) {}
void tearDown (void) {}

void declarationAssignmentSetsValue() {
    SymbolTable* st = newSymbolTable(1);
    ASTNode* ast = newASTIDDeclarationAssignment("n", newASTNumber(1), st).ast;

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarIndex(st, lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void assignmentSetsValue() {
    SymbolTable* st = newSymbolTable(1);
    Symbol* var = insertVar(st, "n");
    ASTNode* ast = newASTAssignment("n", newASTNumber(1), st).ast;

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarIndex(st, var);
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void reassignmentChangesValue() {
    SymbolTable* st = newSymbolTable(1);
    Symbol* var = insertVar(st, "n");
    ASTNode* stmt1 = newASTAssignment("n", newASTNumber(1), st).ast;
    ASTNode* stmt2 = newASTAssignment("n", newASTNumber(2), st).ast;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarIndex(st, var);
    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void assignEvalIDGivesSameValue() {
    SymbolTable* st = newSymbolTable(1);
    ASTNode* stmt1 = newASTIDDeclarationAssignment("n", newASTNumber(1), st).ast;
    ASTNode* stmt2 = newASTIDDeclarationAssignment("m", newASTIDReference("n", st).ast, st).ast;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarIndex(st, lookupVar(st, "m"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}


int main(int argc, char** argv) {
    UNITY_BEGIN();

    RUN_TEST(declarationAssignmentSetsValue);
    RUN_TEST(assignmentSetsValue);
    RUN_TEST(reassignmentChangesValue);
    RUN_TEST(assignEvalIDGivesSameValue);

    return UNITY_END();
}