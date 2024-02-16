#include <stdbool.h>
#include <assert.h>

#include <unity.h>

#include "symbol.h"

void setUp (void) {}
void tearDown (void) {}

void testNewSymbolTable() {
    const int capacity = 1;
    SymbolTable* st = newSymbolTable(capacity);
    TEST_ASSERT_NOT_NULL(st);
    TEST_ASSERT_EQUAL_INT(capacity, getSymbolTableCapacity(st));
    TEST_ASSERT_EQUAL_INT(0, getSymbolTableSize(st));
    deleteSymbolTable(&st);
    TEST_ASSERT_NULL(st);
}

void testInsertVar() {
    const int capacity = 1;
    SymbolTable* st = newSymbolTable(capacity);
    const char* id = "n";
    Symbol* var = insertVar(st, AST_TYPE_INT, id);
    TEST_ASSERT_NOT_NULL(var);
    TEST_ASSERT_EQUAL_STRING(id, getVarId(var));
    TEST_ASSERT_FALSE(isVarInitialized(var));
    deleteSymbolTable(&st);
}

void testResize() {
    const int initial_capacity = 1;
    SymbolTable* st = newSymbolTable(initial_capacity);
    const char* id1 = "n";
    const char* id2 = "m";
    Symbol* var1 = insertVar(st, AST_TYPE_INT, id1);
    Symbol* var2 = insertVar(st, AST_TYPE_INT, id2);
    TEST_ASSERT_EQUAL_INT(2, getSymbolTableSize(st));
    TEST_ASSERT_GREATER_THAN_INT(initial_capacity, getSymbolTableCapacity(st));

    var1 = lookupVar(st, id1);
    var2 = lookupVar(st, id2);
    TEST_ASSERT_NOT_NULL(var1);
    TEST_ASSERT_EQUAL_STRING(id1, getVarId(var1));
    TEST_ASSERT_NOT_NULL(var2);
    TEST_ASSERT_EQUAL_STRING(id2, getVarId(var2));
    deleteSymbolTable(&st);
}

void testLookupVar() {
    const int capacity = 1;
    SymbolTable* st = newSymbolTable(capacity);
    const char* id = "n";
    Symbol* expected_var = insertVar(st, AST_TYPE_INT, id);
    Symbol* actual_var = lookupVar(st, id);
    TEST_ASSERT_EQUAL_PTR(expected_var, actual_var);
    deleteSymbolTable(&st);
}

void testLookupUndefinedVar() {
    const int capacity = 1;
    SymbolTable* st = newSymbolTable(capacity);
    Symbol* actual_var = lookupVar(st, "n");
    TEST_ASSERT_EQUAL_PTR(NULL, actual_var);
    deleteSymbolTable(&st);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testNewSymbolTable);
    RUN_TEST(testInsertVar);
    RUN_TEST(testResize);
    RUN_TEST(testLookupVar);
    RUN_TEST(testLookupUndefinedVar);
    return UNITY_END();
}