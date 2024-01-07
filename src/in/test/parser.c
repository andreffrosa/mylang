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

    deleteASTNode(ast);
    inDelete(ctx);

    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(expected_result, actual_result);
}

void parseAdd() {
    eval_expression(" 1 + 1", 2);
    eval_expression("1 - 2", -1);
}

void parseAddWithFactor() {
    eval_expression("2*2", 4);
    eval_expression("4/2", 2);
    eval_expression("1 + 2*3", 7);
    eval_expression("1 + 4/2", 3);
}

void parseUnaryNegation() {
    eval_expression("-1", -1);
    eval_expression("-2 - 1", -3);
}

void parseParentheses() {
    eval_expression("2*(1+2)", 6);
    eval_expression("(4/2)*2", 4);
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(parseAdd);
  RUN_TEST(parseAddWithFactor);
  RUN_TEST(parseUnaryNegation);
  RUN_TEST(parseParentheses);
  return UNITY_END();
}




