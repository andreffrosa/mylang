#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

ASTNode* ast;
SymbolTable* st;
Frame* frame;

void setUp (void) {
    st = newSymbolTable(1, 3);
}

void tearDown (void) {
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
    deleteFrame(&frame);
}

void evalIfStmtTrue() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmt = res.result_value;

    res = newASTIf(newASTBool(true), stmt);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* cond = res.result_value;

    ast = newASTStatementList(decl, cond);

    frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));
}

void evalIfStmtFalse() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmt = res.result_value;

    res = newASTIf(newASTBool(false), stmt);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* cond = res.result_value;

    ast = newASTStatementList(decl, cond);

    frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, index));
}

void evalIfElseStmtTrue() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmt1 = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(-1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmt2 = res.result_value;

    res = newASTIfElse(newASTBool(true), stmt1, stmt2);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* cond = res.result_value;

    ast = newASTStatementList(decl, cond);

    frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, index));
}

void evalIfElseStmtFalse() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmt1 = res.result_value;

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(-1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* stmt2 = res.result_value;

    res = newASTIfElse(newASTBool(false), stmt1, stmt2);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* cond = res.result_value;

    ast = newASTStatementList(decl, cond);

    frame = executeAST(ast, st);

    unsigned int index = getVarOffset(lookupVar(st, "n"));
    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, index));
}

void evalTernaryCondExpTrue() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* exp1 = res.result_value;
    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    res = newASTAssignment(id_node, newASTInt(-1));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* exp2 = res.result_value;

    res = newASTTernaryCond(newASTBool(true), exp1, exp2);
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, getFrameValue(frame, 0));
}

void evalTernaryCondExpFalse() {
    Symbol* var = defineVar(st, AST_TYPE_INT, "n", false).result_value;

    ASTResult res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    ASTNode* exp1 = newASTAssignment(id_node, newASTInt(1)).result_value;
    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    id_node = newASTID(res.result_value);
    ASTNode* exp2 = newASTAssignment(id_node, newASTInt(-1)).result_value;
    ast = newASTTernaryCond(newASTBool(false), exp1, exp2).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(-1, getFrameValue(frame, getVarOffset(var)));
}

void evalCondAssignment() {
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    ASTNode* n_node = newASTID(n_var);
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    ASTNode* m_node = newASTID(m_var);
    ASTNode* cond = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;
    ast = newASTAssignment(newASTParentheses(cond), newASTInt(2)).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, getVarOffset(n_var), 0);
    setFrameValue(frame, getVarOffset(m_var), 0);
    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, getVarOffset(n_var)));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, getVarOffset(m_var)));
}

void evalNestedCondAssignment() {
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    ASTNode* n_node = newASTID(n_var);
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    ASTNode* m_node = newASTID(m_var);
    Symbol* k_var = defineVar(st, AST_TYPE_INT, "k", false).result_value;
    ASTNode* k_node = newASTID(k_var);
    ast = newASTTernaryCond(newASTBool(true), m_node, k_node).result_value;
    ast = newASTTernaryCond(newASTBool(false), n_node, newASTParentheses(ast)).result_value;
    ast = newASTAssignment(newASTParentheses(ast), newASTInt(2)).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, getVarOffset(n_var), 0);
    setFrameValue(frame, getVarOffset(m_var), 0);
    setFrameValue(frame, getVarOffset(k_var), 0);
    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, getVarOffset(n_var)));
    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, getVarOffset(m_var)));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, getVarOffset(k_var)));
}

void evalChainedCondAssignment() {
    Symbol* n_var = defineVar(st, AST_TYPE_INT, "n", false).result_value;
    ASTNode* n_node = newASTID(n_var);
    Symbol* m_var = defineVar(st, AST_TYPE_INT, "m", false).result_value;
    ASTNode* m_node = newASTID(m_var);
    Symbol* k_var = defineVar(st, AST_TYPE_INT, "k", false).result_value;
    ASTNode* k_node = newASTID(k_var);
    ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;
    ast = newASTAssignment(newASTParentheses(ast), newASTInt(2)).result_value;
    ast = newASTAssignment(k_node, ast).result_value;

    frame = newFrame(getMaxOffset(st) + 1);
    setFrameValue(frame, getVarOffset(n_var), 0);
    setFrameValue(frame, getVarOffset(m_var), 0);
    setFrameValue(frame, getVarOffset(k_var), 0);
    bool result = evalASTExpression(ast, st, frame);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, getVarOffset(n_var)));
    TEST_ASSERT_EQUAL_INT(0, getFrameValue(frame, getVarOffset(m_var)));
    TEST_ASSERT_EQUAL_INT(2, getFrameValue(frame, getVarOffset(k_var)));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(evalIfStmtTrue);
    RUN_TEST(evalIfStmtFalse);
    RUN_TEST(evalIfElseStmtTrue);
    RUN_TEST(evalIfElseStmtFalse);
    RUN_TEST(evalTernaryCondExpTrue);
    RUN_TEST(evalTernaryCondExpFalse);
    RUN_TEST(evalCondAssignment);
    RUN_TEST(evalNestedCondAssignment);
    RUN_TEST(evalChainedCondAssignment);
    return UNITY_END();
}
