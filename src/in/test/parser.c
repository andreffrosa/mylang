#include <unity.h>

#include "in.h"

#include "parser.h"

void setUp (void) {}
void tearDown (void) {}

int evalExp(const char* string) {
    ParseContext ctx = inInitWithString(string);

    ASTNode* ast = NULL;
    bool status = inParse(ctx, &ast);
    int result = evalAST(ast);

    deleteASTNode(&ast);
    inDelete(&ctx);

    TEST_ASSERT_TRUE(status);
    return result;
}

void parseAdditionSequence() {
    TEST_ASSERT_EQUAL_INT(3, evalExp(" 1 + 1+ 1"));
}

void parseSubtraction() {
    TEST_ASSERT_EQUAL_INT(-1, evalExp("1 -2"));
}

void testAdditionAndSubtractionPrecedence() { // They are left associative
    TEST_ASSERT_EQUAL_INT(4, evalExp("2 + 3 - 1"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("2 - 3 + 1"));
}

void parseMultiplicationSequence() {
    TEST_ASSERT_EQUAL_INT(8, evalExp("2*2*2"));
}

void parseDivisionSequence() {
    TEST_ASSERT_EQUAL_INT(1, evalExp("4/2/2"));
}

void parseModulo() {
    TEST_ASSERT_EQUAL_INT(0, evalExp("4%2"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("2%7"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("11%5"));
}

void testMultiplicationDivisionModuloPrecedence() { // They are left associative
    TEST_ASSERT_EQUAL_INT(2, evalExp("4 * 2 / 4"));
    TEST_ASSERT_EQUAL_INT(8, evalExp("4 / 2 * 4"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("4 * 2 % 4"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("4 / 2 % 4"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("4 % 2 * 4"));
}

void testTermAndFactorPrecedence() {
    TEST_ASSERT_EQUAL_INT(7, evalExp("1 + 2*3"));
    TEST_ASSERT_EQUAL_INT(3, evalExp("1 + 4/2"));
    TEST_ASSERT_EQUAL_INT(-5, evalExp("1 - 2*3"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("1 - 4/2"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("1 - 2%3"));
}

void parseUnarySub() {
    TEST_ASSERT_EQUAL_INT(-1, evalExp("-1"));
    TEST_ASSERT_EQUAL_INT(-3, evalExp("-2 - 1"));
}

void parseUnaryAdd() {
    TEST_ASSERT_EQUAL_INT(+1, evalExp("+1"));
    TEST_ASSERT_EQUAL_INT(3, evalExp("+2 + 1"));
}

void parseParentheses() {
    TEST_ASSERT_EQUAL_INT(6, evalExp("2*(1+2)"));
    TEST_ASSERT_EQUAL_INT(4, evalExp("(4/2)*2"));
}

void parseAbs() {
    TEST_ASSERT_EQUAL_INT(1, evalExp("(|-1|)"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("(|1|)"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("(|1-2|)"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("(|2-1|)"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("2*(|1-2|)"));
}

void parseSetPositive() {
    TEST_ASSERT_EQUAL_INT(1, evalExp("+|-1"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("+|1"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("+|-1 + 1"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("+|-1 - 1"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("+|-1 * 2"));
}

void parseSetNegative() {
    TEST_ASSERT_EQUAL_INT(-1, evalExp("-|-1"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("-|1"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("-|-1 + 1"));
    TEST_ASSERT_EQUAL_INT(-2, evalExp("-|-1 - 1"));
    TEST_ASSERT_EQUAL_INT(-2, evalExp("-|-1 * 2"));
}

void parseBitwiseAndSequence() {
    TEST_ASSERT_EQUAL_INT(0, evalExp("1 & 2 & 3"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("2 & 3 & 7"));
}

void parseBitwiseOrSequence() {
    TEST_ASSERT_EQUAL_INT(3, evalExp("1 | 2 | 3"));
    TEST_ASSERT_EQUAL_INT(7, evalExp("2 | 3 | 7"));
}

void parseBitwiseXor() {
    TEST_ASSERT_EQUAL_INT(6, evalExp("5 ^ 3"));
}

void parseBitwiseNot() {
    TEST_ASSERT_EQUAL_INT(-8, evalExp("~7"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("~0"));
}

void parseLeftShift() {
    TEST_ASSERT_EQUAL_INT(20, evalExp("5 << 2"));
}

void parseRightShift() {
    TEST_ASSERT_EQUAL_INT(4, evalExp("16 >> 2"));
}

void testBitwiseHaveSamePrecedences() {
    TEST_ASSERT_EQUAL_INT(3, evalExp("1 & 2 | 3"));
    TEST_ASSERT_EQUAL_INT(4, evalExp("2 | 4 & 5"));

    TEST_ASSERT_EQUAL_INT(3, evalExp("1 & 3 ^ 2"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("1 ^ 2 & 5"));
}

void testBitwiseNotHasMorePrecedence() {
    TEST_ASSERT_EQUAL_INT(2, evalExp("~1 & 2"));
    TEST_ASSERT_EQUAL_INT(2, evalExp("2 & ~1"));
}

void testBitwiseHaveLessPrecedenceThanAlgebraic() {
    TEST_ASSERT_EQUAL_INT(3, evalExp("1 + 2 | 1"));
    TEST_ASSERT_EQUAL_INT(3, evalExp("1 | 2 + 1"));

    TEST_ASSERT_EQUAL_INT(7, evalExp("3 * 2 | 1"));
    TEST_ASSERT_EQUAL_INT(7, evalExp("1 | 3 * 2"));
}

void testShiftPrecedence() { // shift has less than algebraic and than bitwise
    TEST_ASSERT_EQUAL_INT(12, evalExp("1 + 2 << 2"));
    TEST_ASSERT_EQUAL_INT(12, evalExp("3 << 1 + 1"));

    TEST_ASSERT_EQUAL_INT(12, evalExp("3 * 2 << 1"));
    TEST_ASSERT_EQUAL_INT(12, evalExp("3 << 1 * 2"));

    TEST_ASSERT_EQUAL_INT(8, evalExp("2 & 3 << 2"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("1 << 2 & 5"));
}

void testUnaryPrecedenceIsEqual() {
    TEST_ASSERT_EQUAL_INT(2, evalExp("-~1"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("~-1"));

    TEST_ASSERT_EQUAL_INT(1, evalExp("++|-1"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("+|+-1"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("-+|-1"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("+|--1"));

    TEST_ASSERT_EQUAL_INT(-1, evalExp("+-|-1"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("-|+-1"));
    TEST_ASSERT_EQUAL_INT(1, evalExp("--|-1"));
    TEST_ASSERT_EQUAL_INT(-1, evalExp("-|--1"));

    TEST_ASSERT_EQUAL_INT(2, evalExp("+|~1"));
    TEST_ASSERT_EQUAL_INT(-2, evalExp("~+|1"));
    TEST_ASSERT_EQUAL_INT(0, evalExp("~-|1"));
    TEST_ASSERT_EQUAL_INT(-2, evalExp("-|~1"));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(parseAdditionSequence);
    RUN_TEST(parseSubtraction);
    RUN_TEST(testAdditionAndSubtractionPrecedence);
    RUN_TEST(parseMultiplicationSequence);
    RUN_TEST(parseDivisionSequence);
    RUN_TEST(parseModulo);
    RUN_TEST(testMultiplicationDivisionModuloPrecedence);
    RUN_TEST(testTermAndFactorPrecedence);
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
    RUN_TEST(testBitwiseHaveSamePrecedences);
    RUN_TEST(testBitwiseNotHasMorePrecedence);
    RUN_TEST(testBitwiseHaveLessPrecedenceThanAlgebraic);
    RUN_TEST(testShiftPrecedence);
    RUN_TEST(testUnaryPrecedenceIsEqual);
    return UNITY_END();
}
