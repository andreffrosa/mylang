#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <unity.h>

#include "out.h"

#include "test_utils.h"

static SymbolTable* st = NULL;
static ASTNode* ast = NULL;

void setUp (void) {}

void tearDown (void) {
    assert(st == NULL);
    deleteASTNode(&ast);
}

void compileAddSequence() {
    ast = newASTAdd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2 + 3");
}

void compileSubSequence() {
    ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - 2 - 3");
}

void compileSubAdd() {
    ast = newASTAdd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - (2 + 3)"); // Parentheses required
    deleteASTNode(&ast);

    ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2 - 3"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ast = newASTAdd(ast, newASTInt(4)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2 - 3 + 4"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    ast = newASTAdd(ast, newASTInt(4)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2 - 3 + 4"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTSub(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTSub(newASTInt(3), newASTInt(4)).result_value).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - 2 + 3 - 4"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTAdd(newASTInt(3), newASTInt(4)).result_value).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2 - (3 + 4)"); // Parentheses required
}

void compileMulSequence() {
    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1*2*3");
}

void compileMultAdd() {
    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2*3");
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1*2 + 3");
}

void compileMultSub() {
    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - 2*3");
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1*2 - 3");
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(4)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - (2*3 - 4)");
}

void compileDivAdd() {
    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2/3");
    deleteASTNode(&ast);

    ast = newASTDiv(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1/2 + 3");
}

void compileDivSub() {
    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - 2/3");
    deleteASTNode(&ast);

    ast = newASTDiv(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1/2 - 3");
    deleteASTNode(&ast);

    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(4)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - (2/3 - 4)");
}

void compileMulDiv() {
    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1*2/3");
    deleteASTNode(&ast);

    ast = newASTDiv(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1/2*3"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTDiv(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1/(2*3)"); // Parentheses required
}

void compileModAdd() {
    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2%3");
    deleteASTNode(&ast);

    ast = newASTMod(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1%2 + 3");
}

void compileModSub() {
    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - 2%3");
    deleteASTNode(&ast);

    ast = newASTMod(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1%2 - 3");
    deleteASTNode(&ast);

    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(4)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 - (2%3 - 4)");
}

void compileMulMod() {
    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1*2%3");
    deleteASTNode(&ast);

    ast = newASTMod(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1%2*3"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMod(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1%(2*3)"); // Parentheses required
}

void compileDivMod() {
    ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMod(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "4/2%3"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTMod(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTDiv(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "4%2/3"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMod(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1%(2/3)"); // Parentheses required
    deleteASTNode(&ast);

    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTDiv(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1/(2%3)"); // Parentheses required
}

void compileBitwiseOperators() {
    ast = newASTBitwiseAnd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1&2|3"); // No parentheses required, AND has more precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(1|2)&3"); // Parentheses required, AND has more precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseAnd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseXor(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1&2^3"); // No parentheses required, AND has more precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseXor(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(1^2)&3"); // Parentheses required, AND has more precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseXor(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1^2|3"); // No parentheses required, XOR has more precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseXor(ast, newASTInt(3)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(1|2)^3"); // Parentheses required, XOR has more precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseNot(newASTInt(1)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(2)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "~1&2"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTBitwiseNot(newASTInt(1)).result_value;
    ast = newASTBitwiseAnd(newASTInt(2), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "2&~1"); // No parentheses required
}

void compileShiftOperators() {
    ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTLeftShift(ast, newASTInt(2)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2 << 2"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTLeftShift(newASTInt(2), newASTInt(2)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + (2 << 2)"); // Parentheses required
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTLeftShift(newASTInt(3), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "3 << 1 + 1"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(3), newASTInt(2)).result_value;
    ast = newASTLeftShift(ast, newASTInt(1)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "3*2 << 1"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTLeftShift(newASTInt(3), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "3 << 1*2"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTLeftShift(ast, newASTInt(2)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(2&3) << 2"); // Parentheses required
    deleteASTNode(&ast);

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(5)).result_value;
    ast = newASTLeftShift(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 << (2&5)"); // Parentheses required
}

void compileMixedExpression() {
    ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(1)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + 2|1"); // No parentheses required, add has precedence
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(2), newASTInt(1)).result_value;
    ast = newASTBitwiseOr(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1|2 + 1"); // No parentheses required, add has precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseOr(newASTInt(2), newASTInt(1)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "1 + (2|1)"); // No parentheses required, add has precedence
    deleteASTNode(&ast);

    ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(1)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(1|2) + 1"); // Parentheses required
}

void compileLogicalOperators() {
    ast = newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(false)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true && true && false"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalOr(newASTBool(false), newASTBool(false)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "false || false || true"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(false)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true && true || false"); // No parentheses required, && has precedence
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalOr(newASTBool(false), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "false || true && true"); // No parentheses required, && has precedence
    deleteASTNode(&ast);

    ast = newASTLogicalOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true || false) && true"); // Parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalAnd(newASTBool(true), ast).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true && (true || false)"); // Parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalNot(newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "!true");
    deleteASTNode(&ast);

    ast = newASTLogicalNot(newASTLogicalNot(newASTBool(true)).result_value).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "!!true");
    deleteASTNode(&ast);

    ast = newASTLogicalNot(newASTBool(true)).result_value;
    ast = newASTLogicalAnd(newASTBool(true), ast).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(false)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true && !true && false"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalNot(newASTBool(true)).result_value;
    ast = newASTLogicalOr(newASTBool(false), ast).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "false || !true || true"); // No parentheses required
}

void compileBitwiseOperatorsOnBools() {
    ast = newASTBitwiseAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTBitwiseAnd(ast, newASTBool(false)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true&true&false"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTBitwiseOr(newASTBool(false), newASTBool(false)).result_value;
    ast = newASTBitwiseOr(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "false|false|true"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTBitwiseNot(newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "!true"); // Bitwise not on bools is equal to logical not
}

void compileBitwiseAndLogicalOperatorsPrecedence() {
    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseAnd(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true && false)&true");
    deleteASTNode(&ast);

    ast = newASTBitwiseAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true&false && true"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseOr(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true || false)|true");
    deleteASTNode(&ast);

    ast = newASTBitwiseOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true|false || true"); // No parentheses required
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseXor(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true && false)^true");
    deleteASTNode(&ast);

    ast = newASTBitwiseXor(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "true^false || true"); // No parentheses required
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(compileAddSequence);
    RUN_TEST(compileSubAdd);
    RUN_TEST(compileMulSequence);
    RUN_TEST(compileMultAdd);
    RUN_TEST(compileMultSub);
    RUN_TEST(compileDivAdd);
    RUN_TEST(compileDivSub);
    RUN_TEST(compileMulDiv);
    RUN_TEST(compileModAdd);
    RUN_TEST(compileModSub);
    RUN_TEST(compileMulMod);
    RUN_TEST(compileDivMod);
    RUN_TEST(compileBitwiseOperators);
    RUN_TEST(compileShiftOperators);
    RUN_TEST(compileMixedExpression);
    RUN_TEST(compileLogicalOperators);
    RUN_TEST(compileBitwiseOperatorsOnBools);
    RUN_TEST(compileBitwiseAndLogicalOperatorsPrecedence);
    return UNITY_END();
}

