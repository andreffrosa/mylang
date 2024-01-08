#include <unity.h>

#include "in.h"

#include "parser.h"

void setUp (void) {}
void tearDown (void) {}

void eval_expression(const char* string, int expected_result) {
    ParseContext ctx = inInitWithString(string);

    ASTNode* ast = NULL;
    bool status = inParse(ctx, &ast);
    int actual_result = evalAST(ast);

    deleteASTNode(&ast);
    inDelete(&ctx);

    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(expected_result, actual_result);
}

void parseAdditionSequence() {
    eval_expression(" 1 + 1+ 1", 3);
}

void parseSubtraction() {
    eval_expression("1 -2", -1);
}

void parseMultiplicationSequence() {
    eval_expression("2*2*2", 8);
}

void parseDivision() {
    eval_expression("4/2", 2);
}

void parseModulo() {
    eval_expression("4%2", 0);
    eval_expression("2%7", 2);
    eval_expression("11%5", 1);
}

void parseAddWithFactor() {
    eval_expression("1 + 2*3", 7);
    eval_expression("1 + 4/2", 3);
}

void parseUnarySub() {
    eval_expression("-1", -1);
    eval_expression("-2 - 1", -3);
}

void parseUnaryAdd() {
    eval_expression("+1", +1);
    eval_expression("+2 + 1", 3);
}

void parseParentheses() {
    eval_expression("2*(1+2)", 6);
    eval_expression("(4/2)*2", 4);
}

void parseAbs() {
    eval_expression("(|-1|)", 1);
    eval_expression("(|1|)", 1);
    eval_expression("(|1-2|)", 1);
    eval_expression("(|2-1|)", 1);
    eval_expression("2*(|1-2|)", 2);
}

void parseSetPositive() {
    eval_expression("+|-1", 1);
    eval_expression("+|1", 1);
    eval_expression("+|-1 + 1", 2);
    eval_expression("+|-1 - 1", 0);
    eval_expression("+|-1 * 2", 2);
}

void parseSetNegative() {
    eval_expression("-|-1", -1);
    eval_expression("-|1", -1);
    eval_expression("-|-1 + 1", 0);
    eval_expression("-|-1 - 1", -2);
    eval_expression("-|-1 * 2", -2);
}

void testBitwiseAND() {
    eval_expression("5 & 3", 1);
}

void testBitwiseOR() {
    eval_expression("5 | 3", 7);
}

void testBitwiseXOR() {
    eval_expression("5 ^ 3", 6);
}

void testBitwiseNot() {
    eval_expression("~7", -8);
    eval_expression("~0", -1);
}

void testLeftShift() {
    eval_expression("5 << 2", 20);
}

void testRightShift() {
    eval_expression("16 >> 2", 4);
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(parseAdditionSequence);
  RUN_TEST(parseSubtraction);
  RUN_TEST(parseMultiplicationSequence);
  RUN_TEST(parseDivision);
  RUN_TEST(parseModulo);
  RUN_TEST(parseAddWithFactor);
  RUN_TEST(parseUnarySub);
  RUN_TEST(parseUnaryAdd);
  RUN_TEST(parseParentheses);
  RUN_TEST(parseAbs);
  RUN_TEST(parseSetPositive);
  RUN_TEST(parseSetNegative);
  return UNITY_END();
}




