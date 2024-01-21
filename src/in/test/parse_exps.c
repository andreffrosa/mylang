#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

void setUp (void) {}
void tearDown (void) {}

#define ASSERT_MATCH_AST_EXP(str, expected_ast) do {\
    InContext ctx = inInitWithString(str);\
    ASTNode* actual_ast = NULL;\
    bool status = inParse(ctx, (ParseContext){&actual_ast, NULL});\
    TEST_ASSERT_TRUE_MESSAGE(status, "Failed to parse string!");\
    TEST_ASSERT_TRUE_MESSAGE(equalAST(expected_ast, actual_ast), "ASTs are not equal!");\
    deleteASTNode(&expected_ast);\
    deleteASTNode(&actual_ast);\
    inDelete(&ctx);\
} while (0)

void parseAdditionSequence() {
    ASTNode* ast = newASTAdd(newASTAdd(newASTNumber(1), newASTNumber(1)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP(" 1 + 1+ 1", ast);
}

void parseSubtraction() {
    ASTNode* ast = newASTSub(newASTNumber(1), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("1 -2", ast);
}

void addSubSeqIsLeftAssociative() {
    ASTNode* ast = newASTSub(newASTAdd(newASTNumber(2), newASTNumber(3)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("2 + 3 - 1", ast);

    ast = newASTAdd(newASTSub(newASTNumber(2), newASTNumber(3)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("2 - 3 + 1", ast);
}

void parseMultiplicationSequence() {
    ASTNode* ast = newASTMul(newASTMul(newASTNumber(2), newASTNumber(2)), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("2*2*2", ast);
}

void parseDivisionSequence() {
    ASTNode* ast = newASTDiv(newASTDiv(newASTNumber(4), newASTNumber(2)), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("4/2/2", ast);
}

void parseModulo() {
    ASTNode* ast = newASTMod(newASTNumber(4), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("4%2", ast);
}

void mulDivModSeqIsLeftAssociative() {
    ASTNode* ast = newASTDiv(newASTMul(newASTNumber(4), newASTNumber(2)), newASTNumber(4));
    ASSERT_MATCH_AST_EXP("4 * 2 / 4", ast);

    ast = newASTMul(newASTDiv(newASTNumber(4), newASTNumber(2)), newASTNumber(4));
    ASSERT_MATCH_AST_EXP("4 / 2 * 4", ast);

    ast = newASTMod(newASTMul(newASTNumber(4), newASTNumber(2)), newASTNumber(4));
    ASSERT_MATCH_AST_EXP("4 * 2 % 4", ast);

    ast = newASTMod(newASTDiv(newASTNumber(4), newASTNumber(2)), newASTNumber(4));
    ASSERT_MATCH_AST_EXP("4 / 2 % 4", ast);

    ast = newASTMul(newASTMod(newASTNumber(4), newASTNumber(2)), newASTNumber(4));
    ASSERT_MATCH_AST_EXP("4 % 2 * 4", ast);
}

void factorsHaveHigherPrecedenceThanTerms() {
    ASTNode* ast = newASTAdd(newASTNumber(1), newASTMul(newASTNumber(2), newASTNumber(3)));
    ASSERT_MATCH_AST_EXP("1 + 2*3", ast);

    ast = newASTAdd(newASTNumber(1), newASTDiv(newASTNumber(4), newASTNumber(2)));
    ASSERT_MATCH_AST_EXP("1 + 4/2", ast);

    ast = newASTSub(newASTNumber(1), newASTMul(newASTNumber(2), newASTNumber(3)));
    ASSERT_MATCH_AST_EXP("1 - 2*3", ast);

    ast = newASTSub(newASTNumber(1), newASTDiv(newASTNumber(4), newASTNumber(2)));
    ASSERT_MATCH_AST_EXP("1 - 4/2", ast);

    ast = newASTSub(newASTNumber(1), newASTMod(newASTNumber(2), newASTNumber(3)));
    ASSERT_MATCH_AST_EXP("1 - 2%3", ast);
}

void parseUnarySub() {
    ASTNode* ast = newASTUSub(newASTNumber(1));
    ASSERT_MATCH_AST_EXP("-1", ast);

    ast = newASTSub(newASTUSub(newASTNumber(2)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("-2 - 1", ast);
}

void parseUnaryAdd() {
    ASTNode* ast = newASTUAdd(newASTNumber(1));
    ASSERT_MATCH_AST_EXP("+1", ast);

    ast = newASTAdd(newASTUAdd(newASTNumber(2)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("+2 + 1", ast);
}

void parseParentheses() {
    ASTNode* ast = newASTMul(newASTNumber(2), newASTAdd(newASTNumber(1), newASTNumber(2)));
    ASSERT_MATCH_AST_EXP("2*(1+2)", ast);

    ast = newASTMul(newASTDiv(newASTNumber(4), newASTNumber(2)), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("(4/2)*2", ast);
}

void parseAbs() {
    ASTNode* ast = newASTAbs(newASTUSub(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("(|-1|)", ast);

    ast = newASTAbs(newASTNumber(1));
    ASSERT_MATCH_AST_EXP("(|1|)", ast);

    ast = newASTAbs(newASTSub(newASTNumber(1), newASTNumber(2)));
    ASSERT_MATCH_AST_EXP("(|1-2|)", ast);

    ast = newASTAbs(newASTSub(newASTNumber(2), newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("(|2-1|)", ast);

    ast = newASTMul(newASTNumber(2), newASTAbs(newASTSub(newASTNumber(1), newASTNumber(2))));
    ASSERT_MATCH_AST_EXP("2*(|1-2|)", ast);
}

void parseSetPositive() {
    ASTNode* ast = newASTSetPositive(newASTUSub(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("+|-1", ast);

    ast = newASTSetPositive(newASTNumber(1));
    ASSERT_MATCH_AST_EXP("+|1", ast);

    ast = newASTAdd(newASTSetPositive(newASTUSub(newASTNumber(1))), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("+|-1 + 1", ast);

    ast = newASTSub(newASTSetPositive(newASTUSub(newASTNumber(1))), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("+|-1 - 1", ast);

    ast = newASTMul(newASTSetPositive(newASTUSub(newASTNumber(1))), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("+|-1 * 2", ast);
}

void parseSetNegative() {
    ASTNode* ast = newASTSetNegative(newASTUSub(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("-|-1", ast);

    ast = newASTSetNegative(newASTNumber(1));
    ASSERT_MATCH_AST_EXP("-|1", ast);

    ast = newASTAdd(newASTSetNegative(newASTUSub(newASTNumber(1))), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("-|-1 + 1", ast);

    ast = newASTSub(newASTSetNegative(newASTUSub(newASTNumber(1))), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("-|-1 - 1", ast);

    ast = newASTMul(newASTSetNegative(newASTUSub(newASTNumber(1))), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("-|-1 * 2", ast);
}

void parseBitwiseAndSequence() {
    ASTNode* ast = newASTBitwiseAnd(newASTBitwiseAnd(newASTNumber(1), newASTNumber(2)), newASTNumber(3));
    ASSERT_MATCH_AST_EXP("1 & 2 & 3", ast);

    ast = newASTBitwiseAnd(newASTBitwiseAnd(newASTNumber(2), newASTNumber(3)), newASTNumber(7));
    ASSERT_MATCH_AST_EXP("2 & 3 & 7", ast);
}

void parseBitwiseOrSequence() {
    ASTNode* ast = newASTBitwiseOr(newASTBitwiseOr(newASTNumber(1), newASTNumber(2)), newASTNumber(3));
    ASSERT_MATCH_AST_EXP("1 | 2 | 3", ast);

    ast = newASTBitwiseOr(newASTBitwiseOr(newASTNumber(2), newASTNumber(3)), newASTNumber(7));
    ASSERT_MATCH_AST_EXP("2 | 3 | 7", ast);
}

void parseBitwiseXor() {
    ASTNode* ast = newASTBitwiseXor(newASTNumber(5), newASTNumber(3));
    ASSERT_MATCH_AST_EXP("5 ^ 3", ast);
}

void parseBitwiseNot() {
    ASTNode* ast = newASTBitwiseNot(newASTNumber(7));
    ASSERT_MATCH_AST_EXP("~7", ast);

    ast = newASTBitwiseNot(newASTNumber(0));
    ASSERT_MATCH_AST_EXP("~0", ast);
}

void parseLeftShift() {
    ASTNode* ast = newASTLeftShift(newASTNumber(5), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("5 << 2", ast);
}

void parseRightShift() {
    ASTNode* ast = newASTRightShift(newASTNumber(16), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("16 >> 2", ast);
}

void bitwiseOpsHaveSamePrecedence() {
    ASTNode* ast = newASTBitwiseOr(newASTBitwiseAnd(newASTNumber(1), newASTNumber(2)), newASTNumber(3));
    ASSERT_MATCH_AST_EXP("1 & 2 | 3", ast);

    ast = newASTBitwiseAnd(newASTBitwiseOr(newASTNumber(2), newASTNumber(4)), newASTNumber(5));
    ASSERT_MATCH_AST_EXP("2 | 4 & 5", ast);
}

void bitwiseNotHasHigherPrecedence() {
    ASTNode* ast = newASTBitwiseAnd(newASTBitwiseNot(newASTNumber(1)), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("~1 & 2", ast);

    ast = newASTBitwiseAnd(newASTNumber(2), newASTBitwiseNot(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("2 & ~1", ast);
}

void bitwiseOpsHaveLowerPrecedenceThanAlgebraic() {
    ASTNode* ast = newASTBitwiseOr(newASTAdd(newASTNumber(1), newASTNumber(2)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("1 + 2 | 1", ast);

    ast = newASTBitwiseOr(newASTNumber(1), newASTAdd(newASTNumber(2), newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("1 | 2 + 1", ast);

    ast = newASTBitwiseOr(newASTMul(newASTNumber(3), newASTNumber(2)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("3 * 2 | 1", ast);

    ast = newASTBitwiseOr(newASTNumber(1), newASTMul(newASTNumber(3), newASTNumber(2)));
    ASSERT_MATCH_AST_EXP("1 | 3 * 2", ast);
}

void shiftsHaveLowerPrecedenceThanAlgebraicAndBitwise() {
    ASTNode* ast = newASTLeftShift(newASTAdd(newASTNumber(1), newASTNumber(2)), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("1 + 2 << 2", ast);

    ast = newASTLeftShift(newASTNumber(3), newASTAdd(newASTNumber(1), newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("3 << 1 + 1", ast);

    ast = newASTLeftShift(newASTMul(newASTNumber(3), newASTNumber(2)), newASTNumber(1));
    ASSERT_MATCH_AST_EXP("3 * 2 << 1", ast);

    ast = newASTLeftShift(newASTNumber(3), newASTMul(newASTNumber(1), newASTNumber(2)));
    ASSERT_MATCH_AST_EXP("3 << 1 * 2", ast);

    ast = newASTLeftShift(newASTBitwiseAnd(newASTNumber(2), newASTNumber(3)), newASTNumber(2));
    ASSERT_MATCH_AST_EXP("2 & 3 << 2", ast);

    ast = newASTLeftShift(newASTNumber(1), newASTBitwiseAnd(newASTNumber(2), newASTNumber(5)));
    ASSERT_MATCH_AST_EXP("1 << 2 & 5", ast);
}

void unaryPrecedenceIsEqual() {
    ASTNode* ast = newASTUSub(newASTBitwiseNot(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("-~1", ast);

    ast = newASTBitwiseNot(newASTUSub(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("~-1", ast);

    ast = newASTUAdd(newASTSetPositive(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("++|-1", ast);

    ast = newASTSetPositive(newASTUAdd(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("+|+-1", ast);

    ast = newASTUSub(newASTSetPositive(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("-+|-1", ast);

    ast = newASTSetPositive(newASTUSub(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("+|--1", ast);

    ast = newASTUSub(newASTSetPositive(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("-+|-1", ast);

    ast = newASTSetNegative(newASTUAdd(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("-|+-1", ast);

    ast = newASTUSub(newASTSetNegative(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("--|-1", ast);

    ast = newASTSetNegative(newASTUSub(newASTUSub(newASTNumber(1))));
    ASSERT_MATCH_AST_EXP("-|--1", ast);

    ast = newASTSetPositive(newASTBitwiseNot(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("+|~1", ast);

    ast = newASTBitwiseNot(newASTSetPositive(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("~+|1", ast);

    ast = newASTBitwiseNot(newASTSetNegative(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("~-|1", ast);

    ast = newASTSetNegative(newASTBitwiseNot(newASTNumber(1)));
    ASSERT_MATCH_AST_EXP("-|~1", ast);
}

int main(int argc, char** argv) {
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
