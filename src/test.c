#include <stdbool.h>

#include <unity.h>

#include "parser.h"
#include "lexer.h"

void test();

void setUp(void) {}
void tearDown(void) {}

void assert_eval(bool status, int actual_result, int expected_result) {
  TEST_ASSERT_TRUE(status);
  TEST_ASSERT_EQUAL_INT(expected_result, actual_result);
}

void eval_expression(const char *string, int expected_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  YY_BUFFER_STATE input_buffer = yy_scan_string(string, scanner);

  int actual_result;
  bool status = !yyparse(scanner, &actual_result);

  yy_delete_buffer(input_buffer, scanner);
  yylex_destroy(scanner);

  assert_eval(status, actual_result, expected_result);
}

void parser_add_success() {
  eval_expression(" 1 + 1", 2);
  eval_expression("1 - 2", -1);
}

void parser_factor_success() {
  eval_expression("2*2", 4);
  eval_expression("4/2", 2);
  eval_expression("1 + 2*3", 7);
  eval_expression("1 + 4/2", 3);
}

void parser_unary_neg_success() {
  eval_expression("-1", -1);
  eval_expression("-2 - 1", -3);
}

void parser_parenthese_sucess() {
  eval_expression("2*(1+2)", 6);
  eval_expression("(4/2)*2", 4);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(parser_add_success);
  RUN_TEST(parser_factor_success);
  RUN_TEST(parser_unary_neg_success);
  RUN_TEST(parser_parenthese_sucess);
  return UNITY_END();
}