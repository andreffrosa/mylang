#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unity.h>

#include "out.h"

void setUp (void) {}
void tearDown (void) {}

#define PRINT (true)

static inline void testCompile(ASTNode* ast, const char* expected) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);

    outCompileExpression(ast, NULL, stream);
    IOStreamClose(&stream);

    if(PRINT) {printf("%s\n", ptr);}

    TEST_ASSERT_EQUAL_STRING(expected, ptr);

    deleteASTNode(&ast);
    free(ptr);
}

void compileAddSequence() {
    ASTNode* ast = newASTAdd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + 2 + 3");
}

void compileSubSequence() {
    ASTNode* ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2 - 3");
}

void compileSubAdd() {
    ASTNode* ast = newASTAdd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - (2 + 3)"); // Parentheses required

    ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + 2 - 3"); // No parentheses required

    ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ast = newASTAdd(ast, newASTInt(4)).result_value;
    testCompile(ast, "1 + 2 - 3 + 4"); // No parentheses required

    ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    ast = newASTAdd(ast, newASTInt(4)).result_value;
    testCompile(ast, "1 + 2 - 3 + 4"); // No parentheses required

    ast = newASTSub(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTSub(newASTInt(3), newASTInt(4)).result_value).result_value;
    testCompile(ast, "1 - 2 + 3 - 4"); // No parentheses required

    ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTAdd(newASTInt(3), newASTInt(4)).result_value).result_value;
    testCompile(ast, "1 + 2 - (3 + 4)"); // Parentheses required
}

void compileMulSequence() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(newASTInt(1), ast).result_value;
    testCompile(ast, "1*2*3");
}

void compileMultAdd() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + 2*3");

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;
    testCompile(ast, "1*2 + 3");
}

void compileMultSub() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2*3");

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    testCompile(ast, "1*2 - 3");

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(4)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2*3 - 4");
}

void compileDivAdd() {
    ASTNode* ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + 2/3");

    ast = newASTDiv(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;
    testCompile(ast, "1/2 + 3");
}

void compileDivSub() {
    ASTNode* ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2/3");

    ast = newASTDiv(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    testCompile(ast, "1/2 - 3");

    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(4)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2/3 - 4");
}

void compileMulDiv() {
    ASTNode* ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(newASTInt(1), ast).result_value;
    testCompile(ast, "1*2/3");

    ast = newASTDiv(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(3)).result_value;
    testCompile(ast, "1/2*3"); // No parentheses required

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTDiv(newASTInt(1), ast).result_value;
    testCompile(ast, "1/(2*3)"); // Parentheses required
}

void compileModAdd() {
    ASTNode* ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + 2%3");

    ast = newASTMod(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(3)).result_value;
    testCompile(ast, "1%2 + 3");
}

void compileModSub() {
    ASTNode* ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2%3");

    ast = newASTMod(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTSub(ast, newASTInt(3)).result_value;
    testCompile(ast, "1%2 - 3");

    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(4)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    testCompile(ast, "1 - 2%3 - 4");
}

void compileMulMod() {
    ASTNode* ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMul(newASTInt(1), ast).result_value;
    testCompile(ast, "1*2%3");

    ast = newASTMod(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(3)).result_value;
    testCompile(ast, "1%2*3"); // No parentheses required

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMod(newASTInt(1), ast).result_value;
    testCompile(ast, "1%(2*3)"); // Parentheses required
}

void compileDivMod() {
    ASTNode* ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMod(ast, newASTInt(3)).result_value;
    testCompile(ast, "4/2%3"); // No parentheses required

    ast = newASTMod(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTDiv(ast, newASTInt(3)).result_value;
    testCompile(ast, "4%2/3"); // No parentheses required

    ast = newASTDiv(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTMod(newASTInt(1), ast).result_value;
    testCompile(ast, "1%(2/3)"); // Parentheses required

    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTDiv(newASTInt(1), ast).result_value;
    testCompile(ast, "1/(2%3)"); // Parentheses required
}

void compileBitwiseOperators() {
    ASTNode* ast = newASTBitwiseAnd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(3)).result_value;
    testCompile(ast, "1&2|3"); // No parentheses required, AND has more precedence

    ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(3)).result_value;
    testCompile(ast, "(1|2)&3"); // Parentheses required, AND has more precedence

    ast = newASTBitwiseAnd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseXor(ast, newASTInt(3)).result_value;
    testCompile(ast, "1&2^3"); // No parentheses required, AND has more precedence

    ast = newASTBitwiseXor(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(3)).result_value;
    testCompile(ast, "(1^2)&3"); // Parentheses required, AND has more precedence

    ast = newASTBitwiseXor(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(3)).result_value;
    testCompile(ast, "1^2|3"); // No parentheses required, XOR has more precedence

    ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseXor(ast, newASTInt(3)).result_value;
    testCompile(ast, "(1|2)^3"); // Parentheses required, XOR has more precedence

    ast = newASTBitwiseNot(newASTInt(1)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(2)).result_value;
    testCompile(ast, "~1&2"); // No parentheses required

    ast = newASTBitwiseNot(newASTInt(1)).result_value;
    ast = newASTBitwiseAnd(newASTInt(2), ast).result_value;
    testCompile(ast, "2&~1"); // No parentheses required
}

void compileShiftOperators() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTLeftShift(ast, newASTInt(2)).result_value;
    testCompile(ast, "1 + 2 << 2"); // No parentheses required

    ast = newASTLeftShift(newASTInt(2), newASTInt(2)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + (2 << 2)"); // Parentheses required

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTLeftShift(newASTInt(3), ast).result_value;
    testCompile(ast, "3 << 1 + 1"); // No parentheses required

    ast = newASTMul(newASTInt(3), newASTInt(2)).result_value;
    ast = newASTLeftShift(ast, newASTInt(1)).result_value;
    testCompile(ast, "3*2 << 1"); // No parentheses required

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTLeftShift(newASTInt(3), ast).result_value;
    testCompile(ast, "3 << 1*2"); // No parentheses required

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTLeftShift(ast, newASTInt(2)).result_value;
    testCompile(ast, "(2&3) << 2"); // Parentheses required

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(5)).result_value;
    ast = newASTLeftShift(newASTInt(1), ast).result_value;
    testCompile(ast, "1 << (2&5)"); // Parentheses required
}

void compileMixedExpression() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(1)).result_value;
    testCompile(ast, "1 + 2|1"); // No parentheses required, add has precedence

    ast = newASTAdd(newASTInt(2), newASTInt(1)).result_value;
    ast = newASTBitwiseOr(newASTInt(1), ast).result_value;
    testCompile(ast, "1|2 + 1"); // No parentheses required, add has precedence

    ast = newASTBitwiseOr(newASTInt(2), newASTInt(1)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    testCompile(ast, "1 + (2|1)"); // No parentheses required, add has precedence

    ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTAdd(ast, newASTInt(1)).result_value;
    testCompile(ast, "(1|2) + 1"); // Parentheses required
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
    return UNITY_END();
}

