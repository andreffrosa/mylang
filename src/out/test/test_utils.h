#ifndef _OUT_TEST_UTILS_H_
#define _OUT_TEST_UTILS_H_

#include <unity.h>

#include "out/out.h"

extern const OutSerializer cSerializer;
extern const OutSerializer javaSerializer;

const char* compileExp(const ASTNode* ast, const OutSerializer* os, const SymbolTable* st) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTExpression(ast, st, stream, os);
    IOStreamClose(&stream);
    return ptr;
}

#define ASSERT_COMPILE_EQUALS(ast, os, st, str) {\
    const char* ptr = compileExp(ast, os, st);\
    TEST_ASSERT_EQUAL_STRING(str, ptr);\
    free((void*) ptr);\
}

#endif