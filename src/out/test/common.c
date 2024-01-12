#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unity.h>

#include "out.h"

//#include "iostream.h"

void setUp (void) {}
void tearDown (void) {}

// TODO: open_memstream does not work on windows

#define PRINT (true)

static inline void testCompile(ASTNode* ast, const char* expected) {
    char* ptr = NULL;
    size_t size = 0;
    //FILE* out_file = open_memstream(&ptr, &size);
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);

    outCompileExpression(ast, stream);
    //fclose(out_file);
    IOStreamClose(&stream);

    if(PRINT) {printf("%s\n", ptr);}

    TEST_ASSERT_EQUAL_STRING(expected, ptr);

    deleteASTNode(&ast);
    free(ptr);
}

void compileAddSequence() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2 + 3");
}

void compileSubSequence() {
    ASTNode* ast = newASTBinaryOP(AST_SUB, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2 - 3");
}

void compileSubAdd() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - (2 + 3)"); // Parentheses required

    ast = newASTBinaryOP(AST_SUB, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2 - 3"); // No parentheses required

    ast = newASTBinaryOP(AST_SUB, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(4));
    testCompile(ast, "1 + 2 - 3 + 4"); // No parentheses required

    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(4));
    testCompile(ast, "1 + 2 - 3 + 4"); // No parentheses required

    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTBinaryOP(AST_SUB, newASTNumber(3), newASTNumber(4)));
    testCompile(ast, "1 - 2 + 3 - 4"); // No parentheses required

    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTBinaryOP(AST_ADD, newASTNumber(3), newASTNumber(4)));
    testCompile(ast, "1 + 2 - (3 + 4)"); // Parentheses required
}

void compileMulSequence() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), ast);
    testCompile(ast, "1*2*3");
}

void compileMultAdd() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2*3");

    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));
    testCompile(ast, "1*2 + 3");
}

void compileMultSub() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2*3");

    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    testCompile(ast, "1*2 - 3");

    ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(4));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2*3 - 4");
}

void compileDivAdd() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2/3");

    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));
    testCompile(ast, "1/2 + 3");
}

void compileDivSub() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2/3");

    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    testCompile(ast, "1/2 - 3");

    ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(4));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2/3 - 4");
}

void compileMulDiv() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), ast);
    testCompile(ast, "1*2/3");

    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_MUL, ast, newASTNumber(3));
    testCompile(ast, "1/2*3"); // No parentheses required

    ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), ast);
    testCompile(ast, "1/(2*3)"); // Parentheses required
}

void compileModAdd() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2%3");

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));
    testCompile(ast, "1%2 + 3");
}

void compileModSub() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2%3");

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    testCompile(ast, "1%2 - 3");

    ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(4));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2%3 - 4");
}

void compileMulMod() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), ast);
    testCompile(ast, "1*2%3");

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_MUL, ast, newASTNumber(3));
    testCompile(ast, "1%2*3"); // No parentheses required

    ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), ast);
    testCompile(ast, "1%(2*3)"); // Parentheses required
}

void compileDivMod() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(4), newASTNumber(2));
    ast = newASTBinaryOP(AST_MOD, ast, newASTNumber(3));
    testCompile(ast, "4/2%3"); // No parentheses required

    ast = newASTBinaryOP(AST_MOD, newASTNumber(4), newASTNumber(2));
    ast = newASTBinaryOP(AST_DIV, ast, newASTNumber(3));
    testCompile(ast, "4%2/3"); // No parentheses required

    ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), ast);
    testCompile(ast, "1%(2/3)"); // Parentheses required

    ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), ast);
    testCompile(ast, "1/(2%3)"); // Parentheses required
}

void compileBitwiseOperators() {
    ASTNode* ast = newASTBinaryOP(AST_BITWISE_AND, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_OR, ast, newASTNumber(3));
    testCompile(ast, "1&2|3"); // No parentheses required, AND has more precedence

    ast = newASTBinaryOP(AST_BITWISE_OR, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_AND, ast, newASTNumber(3));
    testCompile(ast, "(1|2)&3"); // Parentheses required, AND has more precedence

    ast = newASTBinaryOP(AST_BITWISE_AND, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_XOR, ast, newASTNumber(3));
    testCompile(ast, "1&2^3"); // No parentheses required, AND has more precedence

    ast = newASTBinaryOP(AST_BITWISE_XOR, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_AND, ast, newASTNumber(3));
    testCompile(ast, "(1^2)&3"); // Parentheses required, AND has more precedence

    ast = newASTBinaryOP(AST_BITWISE_XOR, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_OR, ast, newASTNumber(3));
    testCompile(ast, "1^2|3"); // No parentheses required, XOR has more precedence

    ast = newASTBinaryOP(AST_BITWISE_OR, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_XOR, ast, newASTNumber(3));
    testCompile(ast, "(1|2)^3"); // Parentheses required, XOR has more precedence

    ast = newASTUnaryOP(AST_BITWISE_NOT, newASTNumber(1));
    ast = newASTBinaryOP(AST_BITWISE_AND, ast, newASTNumber(2));
    testCompile(ast, "~1&2"); // No parentheses required

    ast = newASTUnaryOP(AST_BITWISE_NOT, newASTNumber(1));
    ast = newASTBinaryOP(AST_BITWISE_AND, newASTNumber(2), ast);
    testCompile(ast, "2&~1"); // No parentheses required
}

void compileShiftOperators() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_L_SHIFT, ast, newASTNumber(2));
    testCompile(ast, "1 + 2 << 2"); // No parentheses required

    ast = newASTBinaryOP(AST_L_SHIFT, newASTNumber(2), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + (2 << 2)"); // Parentheses required

    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(1));
    ast = newASTBinaryOP(AST_L_SHIFT, newASTNumber(3), ast);
    testCompile(ast, "3 << 1 + 1"); // No parentheses required

    ast = newASTBinaryOP(AST_MUL, newASTNumber(3), newASTNumber(2));
    ast = newASTBinaryOP(AST_L_SHIFT, ast, newASTNumber(1));
    testCompile(ast, "3*2 << 1"); // No parentheses required

    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_L_SHIFT, newASTNumber(3), ast);
    testCompile(ast, "3 << 1*2"); // No parentheses required

    ast = newASTBinaryOP(AST_BITWISE_AND, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_L_SHIFT, ast, newASTNumber(2));
    testCompile(ast, "(2&3) << 2"); // Parentheses required

    ast = newASTBinaryOP(AST_BITWISE_AND, newASTNumber(2), newASTNumber(5));
    ast = newASTBinaryOP(AST_L_SHIFT, newASTNumber(1), ast);
    testCompile(ast, "1 << (2&5)"); // Parentheses required
}

void compileMixedExpression() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_BITWISE_OR, ast, newASTNumber(1));
    testCompile(ast, "1 + 2|1"); // No parentheses required, add has precedence

    ast = newASTBinaryOP(AST_ADD, newASTNumber(2), newASTNumber(1));
    ast = newASTBinaryOP(AST_BITWISE_OR, newASTNumber(1), ast);
    testCompile(ast, "1|2 + 1"); // No parentheses required, add has precedence

    ast = newASTBinaryOP(AST_BITWISE_OR, newASTNumber(2), newASTNumber(1));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + (2|1)"); // No parentheses required, add has precedence

    ast = newASTBinaryOP(AST_BITWISE_OR, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(1));
    testCompile(ast, "(1|2) + 1"); // Parentheses required
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(compileAddSequence);
    /*RUN_TEST(compileSubAdd);
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
    RUN_TEST(compileMixedExpression);*/
    return UNITY_END();
}

