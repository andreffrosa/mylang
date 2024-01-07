#include <unity.h>

#include "in.h"

#include "parser.h"
#include "lexer.h"

void setUp (void) {}
void tearDown (void) {}

void scanTokensWithSpaces() {
    ParseContext ctx = inInitWithString(" 1 +1");

    YYSTYPE yylval_param;
    int tok;

    tok = inLex(ctx, &yylval_param);
    TEST_ASSERT_TRUE(tok > 0);
    TEST_ASSERT_EQUAL_INT(NUMBER, tok);
    TEST_ASSERT_EQUAL_INT(1, yylval_param.ival);

    tok = inLex(ctx, &yylval_param);
    TEST_ASSERT_TRUE(tok > 0);
    TEST_ASSERT_EQUAL_INT('+', tok);

    tok = inLex(ctx, &yylval_param);
    TEST_ASSERT_TRUE(tok > 0);
    TEST_ASSERT_EQUAL_INT(NUMBER, tok);
    TEST_ASSERT_EQUAL_INT(1, yylval_param.ival);

    tok = inLex(ctx, &yylval_param);
    TEST_ASSERT_FALSE(tok > 0);

    inDelete(&ctx);
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(scanTokensWithSpaces);
  return UNITY_END();
}

