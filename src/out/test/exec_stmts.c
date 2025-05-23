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
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    ASTNode* ast = newASTAssignment(id_node, newASTInt(1)).result_value;

    Frame* frame = executeAST(ast, st);

    unsigned int index = getVarOffset(var);
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void reassignmentChangesValue() {
    SymbolTable* st = newSymbolTable(1,1);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    ASTNode* stmt1 = newASTAssignment(id_node, newASTInt(1)).result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    ASTNode* stmt2 = newASTAssignment(id_node, newASTInt(2)).result_value;

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
    ASTNode* stmt2 = newASTIDDeclaration(AST_TYPE_INT, "m", newASTID(getVarReference(st, "n").result_value), false, st).result_value;
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
    ASTResult res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    ASTNode* value_node = newASTAdd(copyAST(id_node), newASTInt(1)).result_value;
    ASTNode* restr_exp = newASTAssignment(id_node, value_node).result_value; // valueof(n=n+1)
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

void evalInc() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));

    ASTNode* ast = newASTInc(newASTID(n_var), true).result_value;
    ast = newASTAssignment(newASTID(m_var), ast).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    ast = newASTInc(newASTID(n_var), false).result_value;
    ast = newASTAssignment(newASTID(m_var), ast).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    deleteSymbolTable(&st);
}

void evalDec() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));

    ASTNode* ast = newASTDec(newASTID(n_var), true).result_value;
    ast = newASTAssignment(newASTID(m_var), ast).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    ast = newASTDec(newASTID(n_var), false).result_value;
    ast = newASTAssignment(newASTID(m_var), ast).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    deleteSymbolTable(&st);
}

void evalLogicalToggle() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* z_var = defineVar(st, AST_TYPE_BOOL, "z", false).result_value;
    unsigned int z_index = getVarOffset(lookupVar(st, "z"));
    Symbol* s_var = defineVar(st, AST_TYPE_BOOL, "s", false).result_value;
    unsigned int s_index = getVarOffset(lookupVar(st, "s"));

    ASTNode* ast = newASTLogicalToggle(newASTID(z_var), true).result_value;
    ast = newASTAssignment(newASTID(s_var), ast).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, z_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, z_index));
    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, s_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    ast = newASTLogicalToggle(newASTID(z_var), false).result_value;
    ast = newASTAssignment(newASTID(s_var), ast).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, z_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, z_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, s_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    deleteSymbolTable(&st);
}

void evalBitwiseToggleOfInt() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));

    ASTNode* ast = newASTBitwiseToggle(newASTID(n_var), true).result_value;
    ast = newASTAssignment(newASTID(m_var), ast).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(~0, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(~0, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    ast = newASTBitwiseToggle(newASTID(n_var), false).result_value;
    ast = newASTAssignment(newASTID(m_var), ast).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(~0, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    deleteSymbolTable(&st);
}

void evalBitwiseToggleOfBool() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* z_node = defineVar(st, AST_TYPE_BOOL, "z", false).result_value;
    unsigned int z_index = getVarOffset(lookupVar(st, "z"));
    Symbol* s_node = defineVar(st, AST_TYPE_BOOL, "s", false).result_value;
    unsigned int s_index = getVarOffset(lookupVar(st, "s"));

    ASTNode* ast = newASTBitwiseToggle(newASTID(z_node), true).result_value;
    ast = newASTAssignment(newASTID(s_node), ast).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, z_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, z_index));
    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, s_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    ast = newASTBitwiseToggle(newASTID(z_node), false).result_value;
    ast = newASTAssignment(newASTID(s_node), ast).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, z_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, z_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, s_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    deleteSymbolTable(&st);
}

void evalCompoundAssignmentAdd() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int index = getVarOffset(lookupVar(st, "n"));

    ASTNode* ast = newASTCompoundAssignment(AST_ADD, newASTID(n_var), newASTInt(2)).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalCompoundAssignmentLogicalAnd() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* z_var = defineVar(st, AST_TYPE_INT, "z", false).result_value;
    unsigned int index = getVarOffset(lookupVar(st, "z"));

    ASTNode* ast = newASTCompoundAssignment(AST_LOGICAL_AND, newASTID(z_var), newASTBool(true)).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalConditionalInc() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));
    Symbol* r_var = defineVar(st, AST_TYPE_INT, "r", false).result_value;
    unsigned int r_index = getVarOffset(lookupVar(st, "r"));

    ASTNode* n_node = newASTID(n_var);
    ASTNode* m_node = newASTID(m_var);
    ASTNode* r_node = newASTID(r_var);
    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ASTNode* ast = newASTAssignment(r_node, newASTInc(copyAST(cond), true).result_value).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);
    setFrameValue(frame, m_index, 0);
    setFrameValue(frame, r_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    r_node = newASTID(r_var);
    ast = newASTAssignment(r_node, newASTInc(cond, false).result_value).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);
    setFrameValue(frame, m_index, 0);
    setFrameValue(frame, r_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalConditionalDec() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));
    Symbol* r_var = defineVar(st, AST_TYPE_INT, "r", false).result_value;
    unsigned int r_index = getVarOffset(lookupVar(st, "r"));

    ASTNode* n_node = newASTID(n_var);
    ASTNode* m_node = newASTID(m_var);
    ASTNode* r_node = newASTID(r_var);

    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ASTNode* ast = newASTAssignment(r_node, newASTDec(copyAST(cond), true).result_value).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);
    setFrameValue(frame, m_index, 0);
    setFrameValue(frame, r_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    r_node = newASTID(r_var);
    ast = newASTAssignment(r_node, newASTDec(cond, false).result_value).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);
    setFrameValue(frame, m_index, 0);
    setFrameValue(frame, r_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalConditionalLogicalToggle() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_BOOL, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_BOOL, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));
    Symbol* r_var = defineVar(st, AST_TYPE_BOOL, "r", false).result_value;
    unsigned int r_index = getVarOffset(lookupVar(st, "r"));

    ASTNode* n_node = newASTID(n_var);
    ASTNode* m_node = newASTID(m_var);
    ASTNode* r_node = newASTID(r_var);
    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ASTNode* ast = newASTAssignment(r_node, newASTLogicalToggle(copyAST(cond), true).result_value).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, false);
    setFrameValue(frame, m_index, false);
    setFrameValue(frame, r_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    r_node = newASTID(r_var);
    ast = newASTAssignment(r_node, newASTLogicalToggle(cond, false).result_value).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, false);
    setFrameValue(frame, m_index, false);
    setFrameValue(frame, r_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalConditionalBitwiseToggle() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_BOOL, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_BOOL, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));
    Symbol* r_var = defineVar(st, AST_TYPE_BOOL, "r", false).result_value;
    unsigned int r_index = getVarOffset(lookupVar(st, "r"));

    ASTNode* n_node = newASTID(n_var);
    ASTNode* m_node = newASTID(m_var);
    ASTNode* r_node = newASTID(r_var);
    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ASTNode* ast = newASTAssignment(r_node, newASTBitwiseToggle(copyAST(cond), true).result_value).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, false);
    setFrameValue(frame, m_index, false);
    setFrameValue(frame, r_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);

    r_node = newASTID(r_var);
    ast = newASTAssignment(r_node, newASTBitwiseToggle(cond, false).result_value).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, false);
    setFrameValue(frame, m_index, false);
    setFrameValue(frame, r_index, false);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, m_index));
    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, r_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalConditionalCompoundAssignmentAdd() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));

    ASTNode* n_node = newASTID(n_var);
    ASTNode* m_node = newASTID(m_var);
    ASTNode* ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;
    ast = newASTCompoundAssignment(AST_ADD, newASTParentheses(ast), newASTInt(2)).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, 0);
    setFrameValue(frame, m_index, 0);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, m_index));

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void evalConditionalCompoundAssignmentLogicalAnd() {
    SymbolTable* st = newSymbolTable(1, 2);
    Symbol* n_var = defineVar(st, AST_TYPE_BOOL, "n", false).result_value;
    unsigned int n_index = getVarOffset(lookupVar(st, "n"));
    Symbol* m_var = defineVar(st, AST_TYPE_BOOL, "m", false).result_value;
    unsigned int m_index = getVarOffset(lookupVar(st, "m"));

    ASTNode* n_node = newASTID(n_var);
    ASTNode* m_node = newASTID(m_var);
    ASTNode* ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;
    ast = newASTCompoundAssignment(AST_LOGICAL_AND, newASTParentheses(ast), newASTBool(false)).result_value;

    Frame* frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, n_index, true);
    setFrameValue(frame, m_index, true);

    executeASTStatements(ast, st, frame);

    TEST_ASSERT_EQUAL_INT(false, getFrameValue(frame, n_index));
    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, m_index));

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
    RUN_TEST(evalInc);
    RUN_TEST(evalDec);
    RUN_TEST(evalLogicalToggle);
    RUN_TEST(evalBitwiseToggleOfInt);
    RUN_TEST(evalBitwiseToggleOfBool);
    RUN_TEST(evalCompoundAssignmentAdd);
    RUN_TEST(evalCompoundAssignmentLogicalAnd);
    RUN_TEST(evalConditionalInc);
    RUN_TEST(evalConditionalDec);
    RUN_TEST(evalConditionalLogicalToggle);
    RUN_TEST(evalConditionalCompoundAssignmentAdd);
    RUN_TEST(evalConditionalCompoundAssignmentLogicalAnd);
    return UNITY_END();
}