#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#include "test_utils.h"

void setUp (void) {}
void tearDown (void) {}

void parseEqChain() {
    ASTNode* ast = newASTCmpEQ(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTCmpEQ(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true == true == true", ast);
}

void parseParenthesesOnEqChain() {
    ASTNode* ast = newASTCmpEQ(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTCmpEQ(newASTParentheses(ast), newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("(true == true) == true", ast);
}

void parseEqTypes() {
    ASTNode* ast = newASTCmpEQ(newASTType(AST_TYPE_INT), newASTType(AST_TYPE_BOOL)).result_value;
    ASSERT_MATCH_AST_EXP("int == bool", ast);
}

void parseNeqTypes() {
    ASTNode* ast = newASTCmpNEQ(newASTType(AST_TYPE_INT), newASTType(AST_TYPE_BOOL)).result_value;
    ASSERT_MATCH_AST_EXP("int != bool", ast);
}

void parseLtChain() {
    ASTNode* ast = newASTCmpEQ(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTCmpLT(ast, newASTInt(2)).result_value;
    ast = newASTCmpLTE(ast, newASTInt(3)).result_value;
    ASSERT_MATCH_AST_EXP("1 == 1 < 2 <= 3", ast);
}

void parseGtChain() {
    ASTNode* ast = newASTCmpEQ(newASTInt(3), newASTInt(3)).result_value;
    ast = newASTCmpGT(ast, newASTInt(2)).result_value;
    ast = newASTCmpGTE(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("3 == 3 > 2 >= 1", ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseEqChain);
    RUN_TEST(parseParenthesesOnEqChain);
    RUN_TEST(parseEqTypes);
    RUN_TEST(parseNeqTypes);
    RUN_TEST(parseLtChain);
    RUN_TEST(parseGtChain);
    return UNITY_END();
}