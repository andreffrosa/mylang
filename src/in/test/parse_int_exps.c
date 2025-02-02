#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#include "test_utils.h"

void setUp (void) {}
void tearDown (void) {}

void parseAdditionSequence() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTAdd(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP(" 1 + 1+ 1", ast);
}

void parseSubtraction() {
    ASTNode* ast = newASTSub(newASTInt(1), newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("1 -2", ast);
}

void addSubSeqIsLeftAssociative() {
    ASTNode* ast = newASTAdd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("2 + 3 - 1", ast);

    ast = newASTSub(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("2 - 3 + 1", ast);
}

void parseMultiplicationSequence() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("2*2*2", ast);
}

void parseDivisionSequence() {
    ASTNode* ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTDiv(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("4/2/2", ast);
}

void parseModulo() {
    ASTNode* ast = newASTMod(newASTInt(4), newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("4%2", ast);
}

void mulDivModSeqIsLeftAssociative() {
    ASTNode* ast = newASTMul(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTDiv(ast, newASTInt(4)).result_value;
    ASSERT_MATCH_AST_EXP("4 * 2 / 4", ast);

    ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(4)).result_value;
    ASSERT_MATCH_AST_EXP("4 / 2 * 4", ast);

    ast = newASTMul(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMod(ast, newASTInt(4)).result_value;
    ASSERT_MATCH_AST_EXP("4 * 2 % 4", ast);

    ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMod(ast, newASTInt(4)).result_value;
    ASSERT_MATCH_AST_EXP("4 / 2 % 4", ast);

    ast = newASTMod(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(4)).result_value;
    ASSERT_MATCH_AST_EXP("4 % 2 * 4", ast);
}

void factorsHaveHigherPrecedenceThanTerms() {
    ASTNode* ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 + 2*3", ast);

    ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTAdd(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 + 4/2", ast);

    ast = newASTMul(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 - 2*3", ast);

    ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 - 4/2", ast);

    ast = newASTMod(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTSub(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 - 2%3", ast);
}

void parseUnarySub() {
    ASTNode* ast = newASTUSub(newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("-1", ast);

    ast = newASTSub(newASTUSub(newASTInt(2)).result_value, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("-2 - 1", ast);
}

void parseUnaryAdd() {
    ASTNode* ast = newASTUAdd(newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("+1", ast);

    ast = newASTAdd(newASTUAdd(newASTInt(2)).result_value, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("+2 + 1", ast);
}

void parseParentheses() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTMul(newASTInt(2), ast).result_value;
    ASSERT_MATCH_AST_EXP("2*(1+2)", ast);

    ast = newASTDiv(newASTInt(4), newASTInt(2)).result_value;
    ast = newASTMul(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("(4/2)*2", ast);
}

void parseAbs() {
    ASTNode* ast = newASTAbs(newASTUSub(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("(|-1|)", ast);

    ast = newASTAbs(newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("(|1|)", ast);

    ast = newASTAbs(newASTSub(newASTInt(1), newASTInt(2)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("(|1-2|)", ast);

    ast = newASTAbs(newASTSub(newASTInt(2), newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("(|2-1|)", ast);

    ast = newASTSub(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTMul(newASTInt(2), newASTAbs(ast).result_value).result_value;
    ASSERT_MATCH_AST_EXP("2*(|1-2|)", ast);
}

void parseSetPositive() {
    ASTNode* ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("+|-1", ast);

    ast = newASTSetPositive(newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("+|1", ast);

    ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTAdd(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("+|-1 + 1", ast);

    ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTSub(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("+|-1 - 1", ast);

    ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTMul(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("+|-1 * 2", ast);
}

void parseSetNegative() {
    ASTNode* ast = newASTSetNegative(newASTUSub(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("-|-1", ast);

    ast = newASTSetNegative(newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("-|1", ast);

    ast = newASTSetNegative(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTAdd(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("-|-1 + 1", ast);

    ast = newASTSetNegative(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTSub(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("-|-1 - 1", ast);

    ast = newASTSetNegative(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTMul(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("-|-1 * 2", ast);
}

void parseBitwiseAndSequence() {
    ASTNode* ast = newASTBitwiseAnd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(3)).result_value;
    ASSERT_MATCH_AST_EXP("1 & 2 & 3", ast);

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(7)).result_value;
    ASSERT_MATCH_AST_EXP("2 & 3 & 7", ast);
}

void parseBitwiseOrSequence() {
    ASTNode* ast = newASTBitwiseOr(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(3)).result_value;
    ASSERT_MATCH_AST_EXP("1 | 2 | 3", ast);

    ast = newASTBitwiseOr(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(7)).result_value;
    ASSERT_MATCH_AST_EXP("2 | 3 | 7", ast);
}

void parseBitwiseXor() {
    ASTNode* ast = newASTBitwiseXor(newASTInt(5), newASTInt(3)).result_value;
    ASSERT_MATCH_AST_EXP("5 ^ 3", ast);
}

void parseBitwiseNot() {
    ASTNode* ast = newASTBitwiseNot(newASTInt(7)).result_value;
    ASSERT_MATCH_AST_EXP("~7", ast);

    ast = newASTBitwiseNot(newASTInt(0)).result_value;
    ASSERT_MATCH_AST_EXP("~0", ast);
}

void parseLeftShift() {
    ASTNode* ast = newASTLeftShift(newASTInt(5), newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("5 << 2", ast);
}

void parseRightShift() {
    ASTNode* ast = newASTRightShift(newASTInt(16), newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("16 >> 2", ast);
}

void bitwiseOpsHaveSamePrecedence() {
    ASTNode* ast = newASTBitwiseAnd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(3)).result_value;
    ASSERT_MATCH_AST_EXP("1 & 2 | 3", ast);

    ast = newASTBitwiseOr(newASTInt(2), newASTInt(4)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(5)).result_value;
    ASSERT_MATCH_AST_EXP("2 | 4 & 5", ast);
}

void bitwiseNotHasHigherPrecedence() {
    ASTNode* ast = newASTBitwiseNot(newASTInt(1)).result_value;
    ast = newASTBitwiseAnd(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("~1 & 2", ast);

    ast = newASTBitwiseNot(newASTInt(1)).result_value;
    ast = newASTBitwiseAnd(newASTInt(2), ast).result_value;
    ASSERT_MATCH_AST_EXP("2 & ~1", ast);
}

void bitwiseOpsHaveLowerPrecedenceThanAlgebraic() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("1 + 2 | 1", ast);

    ast = newASTAdd(newASTInt(2), newASTInt(1)).result_value;
    ast = newASTBitwiseOr(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 | 2 + 1", ast);

    ast = newASTMul(newASTInt(3), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("3 * 2 | 1", ast);

    ast = newASTMul(newASTInt(3), newASTInt(2)).result_value;
    ast = newASTBitwiseOr(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 | 3 * 2", ast);
}

void shiftsHaveLowerPrecedenceThanAlgebraicAndBitwise() {
    ASTNode* ast = newASTAdd(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTLeftShift(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("1 + 2 << 2", ast);

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTLeftShift(newASTInt(3), ast).result_value;
    ASSERT_MATCH_AST_EXP("3 << 1 + 1", ast);

    ast = newASTMul(newASTInt(3), newASTInt(2)).result_value;
    ast = newASTLeftShift(ast, newASTInt(1)).result_value;
    ASSERT_MATCH_AST_EXP("3 * 2 << 1", ast);

    ast = newASTMul(newASTInt(1), newASTInt(2)).result_value;
    ast = newASTLeftShift(newASTInt(3), ast).result_value;
    ASSERT_MATCH_AST_EXP("3 << 1 * 2", ast);

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(3)).result_value;
    ast = newASTLeftShift(ast, newASTInt(2)).result_value;
    ASSERT_MATCH_AST_EXP("2 & 3 << 2", ast);

    ast = newASTBitwiseAnd(newASTInt(2), newASTInt(5)).result_value;
    ast = newASTLeftShift(newASTInt(1), ast).result_value;
    ASSERT_MATCH_AST_EXP("1 << 2 & 5", ast);
}

void unaryPrecedenceIsEqual() {
    ASTNode* ast = newASTUSub(newASTBitwiseNot(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("-~1", ast);

    ast = newASTBitwiseNot(newASTUSub(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("~-1", ast);

    ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTUAdd(ast).result_value;
    ASSERT_MATCH_AST_EXP("+ +|-1", ast);

    ast = newASTUAdd(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTSetPositive(ast).result_value;
    ASSERT_MATCH_AST_EXP("+|+-1", ast);

    ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTUSub(ast).result_value;
    ASSERT_MATCH_AST_EXP("-+|-1", ast);

    ast = newASTUSub(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTSetPositive(ast).result_value;
    ASSERT_MATCH_AST_EXP("+|- -1", ast);

    ast = newASTSetPositive(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTUSub(ast).result_value;
    ASSERT_MATCH_AST_EXP("-+|-1", ast);

    ast = newASTUAdd(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTSetNegative(ast).result_value;
    ASSERT_MATCH_AST_EXP("-|+-1", ast);

    ast = newASTSetNegative(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTUSub(ast).result_value;
    ASSERT_MATCH_AST_EXP("- -|-1", ast);

    ast = newASTUSub(newASTUSub(newASTInt(1)).result_value).result_value;
    ast = newASTSetNegative(ast).result_value;
    ASSERT_MATCH_AST_EXP("-|- -1", ast);

    ast = newASTSetPositive(newASTBitwiseNot(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("+|~1", ast);

    ast = newASTBitwiseNot(newASTSetPositive(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("~+|1", ast);

    ast = newASTBitwiseNot(newASTSetNegative(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("~-|1", ast);

    ast = newASTSetNegative(newASTBitwiseNot(newASTInt(1)).result_value).result_value;
    ASSERT_MATCH_AST_EXP("-|~1", ast);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseAdditionSequence);
    RUN_TEST(parseSubtraction);
    RUN_TEST(addSubSeqIsLeftAssociative);
    RUN_TEST(parseMultiplicationSequence);
    RUN_TEST(parseDivisionSequence);
    RUN_TEST(parseModulo);
    RUN_TEST(mulDivModSeqIsLeftAssociative);
    RUN_TEST(factorsHaveHigherPrecedenceThanTerms);
    RUN_TEST(parseUnarySub);
    RUN_TEST(parseUnaryAdd);
    RUN_TEST(parseParentheses);
    RUN_TEST(parseAbs);
    RUN_TEST(parseSetPositive);
    RUN_TEST(parseSetNegative);
    RUN_TEST(parseBitwiseAndSequence);
    RUN_TEST(parseBitwiseOrSequence);
    RUN_TEST(parseBitwiseXor);
    RUN_TEST(parseBitwiseNot);
    RUN_TEST(parseLeftShift);
    RUN_TEST(parseRightShift);
    RUN_TEST(bitwiseOpsHaveSamePrecedence);
    RUN_TEST(bitwiseNotHasHigherPrecedence);
    RUN_TEST(bitwiseOpsHaveLowerPrecedenceThanAlgebraic);
    RUN_TEST(shiftsHaveLowerPrecedenceThanAlgebraicAndBitwise);
    RUN_TEST(unaryPrecedenceIsEqual);
    return UNITY_END();
}
