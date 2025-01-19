#ifndef _OUT_TEST_UTILS_H_
#define _OUT_TEST_UTILS_H_

#include <unity.h>

#include "out/out.h"

extern const OutSerializer cSerializer;
extern const OutSerializer javaSerializer;

const char* compileExpFull(const ASTNode* ast, const OutSerializer* os, const SymbolTable* st) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTExpression(ast, st, stream, os);
    IOStreamClose(&stream);
    return ptr;
}

#define ASSERT_COMPILE_EQUALS(ast, os, st, str) {\
    const char* ptr = compileExpFull(ast, os, st);\
    TEST_ASSERT_EQUAL_STRING(str, ptr);\
    free((void*) ptr);\
}

const char* compileStmt(const ASTNode* ast, const OutSerializer* os, const SymbolTable* st) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTStatements(ast, st, stream, os, 0, true);
    IOStreamClose(&stream);
    return ptr;
}

const char* compileExp(const ASTNode* ast, const SymbolTable* st) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTExpression(ast, st, stream, &cSerializer);
    IOStreamClose(&stream);
    return ptr;
}

#define ASSERT_COMPILE_STMT_EQUALS(ast, os, str) {\
    const char* txt = compileStmt(ast, os, st);\
    TEST_ASSERT_NOT_NULL(txt);\
    TEST_ASSERT_EQUAL_STRING(str, txt);\
    free((void*)txt);\
}

#define ASSERT_COMPILE_EXP_EQUALS(ast, str) {\
    const char* txt = compileExp(ast, st);\
    TEST_ASSERT_NOT_NULL(txt);\
    TEST_ASSERT_EQUAL_STRING(str, txt);\
    free((void*)txt);\
}

#endif