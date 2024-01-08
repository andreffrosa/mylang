#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unity.h>

#include "out.h"

void setUp (void) {}
void tearDown (void) {}

static inline void testCompile(ASTNode* ast, const char* expected, bool print) {
    char* ptr = NULL;
    size_t sizeloc = 0;
    FILE* out_file = open_memstream(&ptr, &sizeloc);

    outCompileExpression(ast, out_file);
    fclose(out_file);

    if(print) {printf("%s\n", ptr);}

    //TEST_ASSERT_NOT_NULL(strstr(ptr, expected));
    TEST_ASSERT_EQUAL_STRING(ptr, expected);

    deleteASTNode(&ast);
    free(ptr);
}

void compileAddSequence() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2 + 3", true);
}

void compileSubSequence() {
    ASTNode* ast = newASTBinaryOP(AST_SUB, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2 - 3", true);
}

void compileSubAdd() {
    ASTNode* ast = newASTBinaryOP(AST_ADD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - (2 + 3)", true); // Parentheses required

    ast = newASTBinaryOP(AST_SUB, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2 - 3", true); // No parentheses required

    ast = newASTBinaryOP(AST_SUB, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(4));
    testCompile(ast, "1 + 2 - 3 + 4", true); // No parentheses required

    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(4));
    testCompile(ast, "1 + 2 - 3 + 4", true); // No parentheses required

    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTBinaryOP(AST_SUB, newASTNumber(3), newASTNumber(4)));
    testCompile(ast, "1 - 2 + 3 - 4", true); // No parentheses required

    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTBinaryOP(AST_ADD, newASTNumber(3), newASTNumber(4)));
    testCompile(ast, "1 + 2 - (3 + 4)", true); // Parentheses required
}

void compileMulSequence() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), ast);
    testCompile(ast, "1*2*3", true);
}

void compileMultAdd() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2*3", true);

    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));
    testCompile(ast, "1*2 + 3", true);
}

void compileMultSub() {
    ASTNode* ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2*3", true);

    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    testCompile(ast, "1*2 - 3", true);

    ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(4));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2*3 - 4", true);
}

void compileDivAdd() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2/3", true);

    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));
    testCompile(ast, "1/2 + 3", true);
}

void compileDivSub() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2/3", true);

    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    testCompile(ast, "1/2 - 3", true);

    ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(4));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2/3 - 4", true);
}

void compileMulDiv() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), ast);
    testCompile(ast, "1*2/3", true);

    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_MUL, ast, newASTNumber(3));
    testCompile(ast, "1/2*3", true); // No parentheses required

    ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), ast);
    testCompile(ast, "1/(2*3)", true); // Parentheses required
    // TODO
}

// TODO: mul_mod div_mod

void compileModAdd() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_ADD, newASTNumber(1), ast);
    testCompile(ast, "1 + 2%3", true);

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_ADD, ast, newASTNumber(3));
    testCompile(ast, "1%2 + 3", true);
}

void compileModSub() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2%3", true);

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(3));
    testCompile(ast, "1%2 - 3", true);

    ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_SUB, ast, newASTNumber(4));
    ast = newASTBinaryOP(AST_SUB, newASTNumber(1), ast);
    testCompile(ast, "1 - 2%3 - 4", true);
}

void compileMulMod() {
    ASTNode* ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MUL, newASTNumber(1), ast);
    testCompile(ast, "1*2%3", true);

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_MUL, ast, newASTNumber(3));
    testCompile(ast, "1%2*3", true); // No parentheses required

    ast = newASTBinaryOP(AST_MUL, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), ast);
    testCompile(ast, "1%(2*3)", true); // Parentheses required
    // TODO
}

void compileDivMod() {
    ASTNode* ast = newASTBinaryOP(AST_DIV, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_MOD, ast, newASTNumber(3));
    testCompile(ast, "(1/2)%3", true);

    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), newASTNumber(2));
    ast = newASTBinaryOP(AST_DIV, ast, newASTNumber(3));
    testCompile(ast, "(1%2)/3", true); // No parentheses required

    ast = newASTBinaryOP(AST_DIV, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_MOD, newASTNumber(1), ast);
    testCompile(ast, "1%(2/3)", true); // Parentheses required

    ast = newASTBinaryOP(AST_MOD, newASTNumber(2), newASTNumber(3));
    ast = newASTBinaryOP(AST_DIV, newASTNumber(1), ast);
    testCompile(ast, "1/(2%3)", true); // Parentheses required
    // TODO
}

// TODO: rename
void compileNestedExpWithAbs() {
    ASTNode* add_node = newASTBinaryOP(AST_ADD, newASTNumber(1), newASTNumber(2));
    ASTNode* div_node = newASTBinaryOP(AST_DIV, newASTNumber(25), newASTNumber(5));
    ASTNode* abs_node = newASTUnaryOP(AST_ABS, newASTUnaryOP(AST_USUB, div_node));
    ASTNode* mod_node = newASTBinaryOP(AST_MOD, abs_node, newASTNumber(7));
    ASTNode* ast = newASTBinaryOP(AST_SUB, mod_node, add_node);

    testCompile(ast, "abs(-(25/5))%7 - (1 + 2)", true);
}

int main(int argc, char** argv) {
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
    // RUN_TEST(compileDivMod);
    // RUN_TEST(compileNestedExpWithAbs);
    return UNITY_END();
}

