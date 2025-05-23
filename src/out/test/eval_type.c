#include <unity.h>

#include <assert.h>

#include "ast/ast.h"
#include "out/out.h"

void setUp (void) {}
void tearDown (void) {}

void typeOfIntIsInt() {
    ASTNode* ast = newASTTypeOf(newASTInt(1));

    int result = evalASTExpression(ast, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_INT, result);

    deleteASTNode(&ast);
}

void typeOfIntExpressionIsInt() {
    ASTNode* ast = newASTTypeOf(newASTAdd(newASTInt(1), newASTInt(1)).result_value);

    int result = evalASTExpression(ast, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_INT, result);

    deleteASTNode(&ast);
}

void typeOfBoolExpressionIsBool() {
    ASTNode* ast = newASTTypeOf(newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value);

    int result = evalASTExpression(ast, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_BOOL, result);

    deleteASTNode(&ast);
}

void typeOfBoolIsBool() {
    ASTNode* ast = newASTTypeOf(newASTBool(true));

    int result = evalASTExpression(ast, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_BOOL, result);

    deleteASTNode(&ast);
}

void typeofTypeIsType() {
    ASTNode* ast = newASTTypeOf(newASTType(AST_TYPE_INT));

    int result = evalASTExpression(ast, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_TYPE, result);

    deleteASTNode(&ast);
}

void typeofIDIsVarType() {
    SymbolTable* st = newSymbolTable(1, 1);
    Symbol* var = defineVar(st, AST_TYPE_BOOL, "n", false).result_value;
    ASTNode* ast = newASTTypeOf(newASTID(var));

    Frame* frame = newFrame(1);
    setFrameValue(frame, 0, false);

    int result = evalASTExpression(ast, st, frame);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_BOOL, result);

    deleteASTNode(&ast);
    deleteSymbolTable(&st);
    deleteFrame(&frame);
}

void typeofValueofRestrainedExpIsCorrectType() {
    SymbolTable* st = newSymbolTable(1, 1);
    Symbol* var = defineVar(st, AST_TYPE_BOOL, "n", false).result_value;
    ASTNode* id_node = newASTID(var);
    ASTResult res = newASTAssignment(id_node, newASTBool(true));
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* ast = newASTTypeOf(res.result_value);

    Frame* frame = newFrame(1);
    setFrameValue(frame, 0, false);

    int result = evalASTExpression(ast, st, frame);
    TEST_ASSERT_EQUAL_INT(AST_TYPE_BOOL, result);
    TEST_ASSERT_EQUAL_INT(true, getFrameValue(frame, 0));

    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(typeOfIntIsInt);
    RUN_TEST(typeOfIntExpressionIsInt);
    RUN_TEST(typeOfBoolIsBool);
    RUN_TEST(typeOfBoolExpressionIsBool);
    RUN_TEST(typeofTypeIsType);
    RUN_TEST(typeofIDIsVarType);
    RUN_TEST(typeofValueofRestrainedExpIsCorrectType);
    return UNITY_END();
}