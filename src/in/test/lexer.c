#include <unity.h>

#include "in.h"

#include "parser.h"
#include "lexer.h"

void setUp (void) {}
void tearDown (void) {}

#define TOKENIZE(str) {\
    InContext ctx = inInitWithString(str);\
    YYSTYPE yylval_param;\
    int tok;\

#define GET_NEXT_TOKEN(ctx) (tok = inLex(ctx, &yylval_param))

#define ASSERT_TOKEN_IS_INT(val) do {\
    GET_NEXT_TOKEN(ctx);\
    TEST_ASSERT_TRUE(tok > 0);\
    TEST_ASSERT_EQUAL_INT(INT_LITERAL, tok);\
    TEST_ASSERT_EQUAL_INT(val, yylval_param.ival);\
} while(0)

#define ASSERT_TOKEN_IS_BOOL(val) do {\
    GET_NEXT_TOKEN(ctx);\
    TEST_ASSERT_TRUE(tok > 0);\
    TEST_ASSERT_EQUAL_INT(BOOL_LITERAL, tok);\
    TEST_ASSERT_EQUAL_INT(val, yylval_param.bval);\
} while(0)

#define ASSERT_TOKEN_IS_OP(op_type) do {\
    GET_NEXT_TOKEN(ctx);\
    TEST_ASSERT_TRUE(tok > 0);\
    TEST_ASSERT_EQUAL_INT(op_type, tok);\
} while(0)

#define ASSERT_TOKEN_IS_ID(id) do {\
    GET_NEXT_TOKEN(ctx);\
    TEST_ASSERT_TRUE(tok > 0);\
    TEST_ASSERT_EQUAL_INT(ID, tok);\
    TEST_ASSERT_EQUAL_STRING(id, yylval_param.sval);\
} while(0)

#define ASSERT_NO_MORE_TOKENS() do {\
    GET_NEXT_TOKEN(ctx);\
    TEST_ASSERT_FALSE(tok > 0);\
    inDelete(&ctx);\
} while(0); }


void scanTokensWithSpaces() {
    TOKENIZE(" 1 +1");

    ASSERT_TOKEN_IS_INT(1);
    ASSERT_TOKEN_IS_OP('+');
    ASSERT_TOKEN_IS_INT(1);
    ASSERT_NO_MORE_TOKENS();
}

void scanAbs() {
    TOKENIZE("(|1|)");

    ASSERT_TOKEN_IS_OP(OPEN_ABS);
    ASSERT_TOKEN_IS_INT(1);
    ASSERT_TOKEN_IS_OP(CLOSE_ABS);
    ASSERT_NO_MORE_TOKENS();
}

void scanSetPositive() {
    TOKENIZE("+|1");

    ASSERT_TOKEN_IS_OP(SET_POSITIVE);
    ASSERT_TOKEN_IS_INT(1);
    ASSERT_NO_MORE_TOKENS();
}

void scanHexNumber() {
    TOKENIZE("0xa");

    ASSERT_TOKEN_IS_INT(10);
    ASSERT_NO_MORE_TOKENS();

    TOKENIZE("0xf");

    ASSERT_TOKEN_IS_INT(15);
    ASSERT_NO_MORE_TOKENS();

    TOKENIZE("0x10");

    ASSERT_TOKEN_IS_INT(16);
    ASSERT_NO_MORE_TOKENS();
}

void scanID() {
    TOKENIZE(" id ");

    ASSERT_TOKEN_IS_ID("id");
    ASSERT_NO_MORE_TOKENS();
}

void scanIDWithNumber() {
    TOKENIZE(" id2 ");

    ASSERT_TOKEN_IS_ID("id2");
    ASSERT_NO_MORE_TOKENS();
}

void scanIDWithUnderscore() {
    TOKENIZE(" id_ ");

    ASSERT_TOKEN_IS_ID("id_");
    ASSERT_NO_MORE_TOKENS();
}

void scanBoolValues() {
    TOKENIZE("true");
    ASSERT_TOKEN_IS_BOOL(true);
    ASSERT_NO_MORE_TOKENS();

    TOKENIZE("false");
    ASSERT_TOKEN_IS_BOOL(false);
    ASSERT_NO_MORE_TOKENS();
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(scanTokensWithSpaces);
    RUN_TEST(scanAbs);
    RUN_TEST(scanSetPositive);
    RUN_TEST(scanHexNumber);
    RUN_TEST(scanID);
    RUN_TEST(scanIDWithNumber);
    RUN_TEST(scanIDWithUnderscore);
    RUN_TEST(scanBoolValues);
    return UNITY_END();
}

