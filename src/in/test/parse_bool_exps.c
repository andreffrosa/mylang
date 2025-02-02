#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#include "test_utils.h"

void setUp (void) {}
void tearDown (void) {}

void parseLogicalAndSequence() {
    ASTNode* ast = newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true && true && true", ast);
}

void parseLogicalOrSequence() {
    ASTNode* ast = newASTLogicalOr(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true || true || true", ast);
}

void logicalOrAndLogicalAndHaveSamePrecendence() { // Different from C and Java
    ASTNode* ast = newASTLogicalAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true && true || true", ast);

    ast = newASTLogicalOr(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true || true && true", ast);
}

void parseLogicalNot() {
    ASTNode* ast = newASTLogicalNot(newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("!true", ast);

    ast = newASTLogicalNot(newASTLogicalNot(newASTBool(true)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("! !true", ast);
}

void logicalNotHasHigherPrecedence() {
    ASTNode* ast = newASTLogicalNot(newASTBool(true)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("!true && true", ast);

    ast = newASTLogicalNot(newASTBool(true)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("!true || true", ast);
}

void parseBitwiseAndSequence() {
    ASTNode* ast = newASTBitwiseAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseAnd(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true & false & true", ast);
}

void parseBitwiseOrSequence() {
    ASTNode* ast = newASTBitwiseOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true | false | true", ast);
}

void parseBitwiseXor() {
    ASTNode* ast = newASTBitwiseXor(newASTBool(true), newASTBool(false)).result_value;
    ASSERT_MATCH_AST_EXP("true ^ false", ast);
}

void parseBitwiseNot() {
    ASTNode* ast = newASTBitwiseNot(newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("~true", ast);

    ast = newASTBitwiseNot(newASTBool(false)).result_value;
    ASSERT_MATCH_AST_EXP("~false", ast);
}

void bitwiseOpsHaveSamePrecedence() {
    ASTNode* ast = newASTBitwiseAnd(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTBitwiseOr(ast, newASTBool(false)).result_value;
    ASSERT_MATCH_AST_EXP("true & true | false", ast);

    ast = newASTBitwiseOr(newASTBool(true), newASTBool(true)).result_value;
    ast = newASTBitwiseAnd(ast, newASTBool(false)).result_value;
    ASSERT_MATCH_AST_EXP("true | true & false", ast);
}

void bitwiseAndLogicalOperatorsHaveSamePrecedence() {
    ASTNode* ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseAnd(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true && false & true", ast);

    ast = newASTBitwiseAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalAnd(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true & false && true", ast);

    ast = newASTLogicalOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true || false | true", ast);

    ast = newASTBitwiseOr(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true | false || true", ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTBitwiseXor(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true && false ^ true", ast);

    ast = newASTBitwiseXor(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTLogicalOr(ast, newASTBool(true)).result_value;
    ASSERT_MATCH_AST_EXP("true ^ false || true", ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseLogicalAndSequence);
    RUN_TEST(parseLogicalOrSequence);
    RUN_TEST(logicalOrAndLogicalAndHaveSamePrecendence);
    RUN_TEST(parseLogicalNot);
    RUN_TEST(logicalNotHasHigherPrecedence);
    RUN_TEST(parseBitwiseAndSequence);
    RUN_TEST(parseBitwiseOrSequence);
    RUN_TEST(parseBitwiseXor);
    RUN_TEST(parseBitwiseNot);
    RUN_TEST(bitwiseOpsHaveSamePrecedence);
    RUN_TEST(bitwiseAndLogicalOperatorsHaveSamePrecedence);
    return UNITY_END();
}
