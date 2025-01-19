#include "symbol.h"

#include <assert.h>
#include <stdbool.h>
#include <unity.h>

static SymbolTable* st;

void setUp(void) { st = NULL; }

void tearDown(void) {
    if (st != NULL) {
        deleteSymbolTable(&st);
    }
}

#define TABLE_CAPACITY 1
#define SCOPE_CAPACITY 1

void newSymbolTableReturnsValidEmptyTable() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    TEST_ASSERT_NOT_NULL(st);
    TEST_ASSERT_EQUAL_INT(TABLE_CAPACITY, getSymbolTableCapacity(st));
    TEST_ASSERT_EQUAL_INT(1, getSymbolTableSize(st));
    TEST_ASSERT_EQUAL_INT(0, getMaxOffset(st));
    TEST_ASSERT_EQUAL_INT(0, getTotalSymbolAmount(st));

    Scope* root = getScope(st, 0);
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_INT(SCOPE_CAPACITY, getScopeCapacity(root));
    TEST_ASSERT_EQUAL_INT(0, getScopeSize(root));
}

void deleteSymbolTableSetsVarNull() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    deleteSymbolTable(&st);
    TEST_ASSERT_NULL(st);
}

void newVarHasZeroRedefLevel() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_EQUAL_INT(0, getVarRedefLevel(res.result_value));
}

void variablesAreRetrievableAfterResizeOfTable() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    enterScope(st, SCOPE_CAPACITY);

    TEST_ASSERT_GREATER_OR_EQUAL_INT(TABLE_CAPACITY, getSymbolTableCapacity(st));
    TEST_ASSERT_EQUAL_INT(2, getSymbolTableSize(st));

    Symbol* n = lookupVar(st, "n");
    TEST_ASSERT_NOT_NULL(n);
}

void variablesAreRetrievableAfterResizeOfScope() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    res = defineVar(st, AST_TYPE_INT, "m", false);
    TEST_ASSERT_TRUE(isOK(res));

    Scope* scope = getCurrentScope(st);
    TEST_ASSERT_NOT_NULL(scope);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(SCOPE_CAPACITY, getScopeCapacity(scope));
    TEST_ASSERT_EQUAL_INT(2, getScopeSize(scope));

    Symbol* n = lookupVar(st, "n");
    TEST_ASSERT_NOT_NULL(n);
    Symbol* m = lookupVar(st, "m");
    TEST_ASSERT_NOT_NULL(m);
}

void defineVarIncreasesMaxOffsetAndTotalSymbolAmount() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res;

    res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_EQUAL_INT(0, getMaxOffset(st));
    TEST_ASSERT_EQUAL_INT(1, getTotalSymbolAmount(st));

    res = defineVar(st, AST_TYPE_INT, "m", false);
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_EQUAL_INT(1, getMaxOffset(st));
    TEST_ASSERT_EQUAL_INT(2, getTotalSymbolAmount(st));

    res = defineVar(st, AST_TYPE_INT, "k", false);
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_EQUAL_INT(2, getMaxOffset(st));
    TEST_ASSERT_EQUAL_INT(3, getTotalSymbolAmount(st));
}

void lookupDefinedVarReturnsVar() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    Symbol* var = lookupVar(st, "n");
    TEST_ASSERT_NOT_NULL(var);
    TEST_ASSERT_EQUAL_STRING("n", getVarId(var));
}

void lookupVarDefinedInParentScopeReturnsVar() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    enterScope(st, SCOPE_CAPACITY);
    Symbol* var = lookupVar(st, "n");

    TEST_ASSERT_NOT_NULL(var);
    TEST_ASSERT_EQUAL_STRING("n", getVarId(var));
}

void lookupVarDefinedInChildScopeReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    enterScope(st, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_TRUE(leaveScope(st));

    Symbol* var = lookupVar(st, "n");

    TEST_ASSERT_NULL(var);
}

void redefineVarInSameScopeWithoutRedefReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}

void redefineVarInSameScopeWithRedefReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}

void defineNewVarWithRedefReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);

    ASTResult res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}

void redefinedVarInChildScopeWithoutRedefReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    enterScope(st, SCOPE_CAPACITY);

    res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_ALREADY_DEFINED, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}

void redefineVarInChildScopeWithRedefRetursOk() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));

    enterScope(st, SCOPE_CAPACITY);

    res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;

    TEST_ASSERT_EQUAL_STRING("n", getVarId(var));
}

void redefinedVarsInNestedScopesHaveHigherRedefLevel() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;
    TEST_ASSERT_EQUAL_INT(0, getVarRedefLevel(var));

    enterScope(st, SCOPE_CAPACITY);

    res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isOK(res));
    var = res.result_value;
    TEST_ASSERT_EQUAL_INT(1, getVarRedefLevel(var));

    enterScope(st, SCOPE_CAPACITY);

    res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isOK(res));
    var = res.result_value;
    TEST_ASSERT_EQUAL_INT(2, getVarRedefLevel(var));
}

void redefinedVarsInConcurrentScopesHaveSameRedefLevel() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;
    TEST_ASSERT_EQUAL_INT(0, getVarRedefLevel(var));

    enterScope(st, SCOPE_CAPACITY);

    res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isOK(res));
    var = res.result_value;
    TEST_ASSERT_EQUAL_INT(1, getVarRedefLevel(var));

    TEST_ASSERT_TRUE(leaveScope(st));
    enterScope(st, SCOPE_CAPACITY);

    res = defineVar(st, AST_TYPE_INT, "n", true);
    TEST_ASSERT_TRUE(isOK(res));
    var = res.result_value;
    TEST_ASSERT_EQUAL_INT(1, getVarRedefLevel(var));
}

void leaveRootScopeFails() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    TEST_ASSERT_FALSE(leaveScope(st));
}

void getInvalidScopeIndexReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    Scope* scope = getScope(st, 1);
    TEST_ASSERT_NULL(scope);
}

void getLeftReferenceOfUndefinedVarReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}

void getLeftReferenceOfDefinedVarReturnsOk() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;
    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_EQUAL_PTR(var, res.result_value);
}

void getRightReferenceOfUndefinedVarReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_DEFINED, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}

/*
void getRightReferenceOfDefinedUninitializedVarReturnsErr() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isERR(res));
    TEST_ASSERT_EQUAL_INT(AST_RES_ERR_ID_NOT_INIT, res.result_type);
    TEST_ASSERT_EQUAL_STRING("n", res.result_value);
}
*/

void getRightReferenceOfDefinedInitializedVarReturnsOk() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var = res.result_value;
    res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    TEST_ASSERT_EQUAL_PTR(var, res.result_value);
}

void offsetOfVarsInNestedScopesIncreases() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var_n = res.result_value;
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var_m = res.result_value;
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "k", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var_k = res.result_value;

    TEST_ASSERT_TRUE(getVarOffset(var_m) > getVarOffset(var_n));
    TEST_ASSERT_TRUE(getVarOffset(var_k) > getVarOffset(var_m));
    TEST_ASSERT_EQUAL_INT(getVarOffset(var_k), getMaxOffset(st));
}

void offsetOfVarsInConcurrentScopesIsEqual() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var_n = res.result_value;
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var_m = res.result_value;
    TEST_ASSERT_TRUE(leaveScope(st));
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "k", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* var_k = res.result_value;

    TEST_ASSERT_TRUE(getVarOffset(var_m) > getVarOffset(var_n));
    TEST_ASSERT_EQUAL_INT(getVarOffset(var_k), getVarOffset(var_m));
    TEST_ASSERT_EQUAL_INT(getVarOffset(var_k), getMaxOffset(st));
}

void cloneEmptyTableReturnsNewEmptyTable() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    SymbolTable* clone_st = newSymbolTableClone(st);
    TEST_ASSERT_NOT_NULL(clone_st);
    TEST_ASSERT_EQUAL_INT(getMaxOffset(st), getMaxOffset(clone_st));
    TEST_ASSERT_EQUAL_INT(getTotalSymbolAmount(st), getTotalSymbolAmount(clone_st));
}

void cloneTableReturnsIdenticalTable() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_var = res.result_value;

    SymbolTable* clone_st = newSymbolTableClone(st);
    TEST_ASSERT_NOT_NULL(clone_st);
    TEST_ASSERT_EQUAL_INT(getMaxOffset(st), getMaxOffset(clone_st));
    TEST_ASSERT_EQUAL_INT(getTotalSymbolAmount(st), getTotalSymbolAmount(clone_st));

    Symbol* clone_var = lookupVar(clone_st, "n");
    TEST_ASSERT_NOT_NULL(clone_var);
    TEST_ASSERT_TRUE(src_var != clone_var);
}

void cloneTableWithNestedScopesReturnsIdenticalTable() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_n = res.result_value;
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_m = res.result_value;
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "k", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_k = res.result_value;

    SymbolTable* clone_st = newSymbolTableClone(st);
    TEST_ASSERT_NOT_NULL(clone_st);
    TEST_ASSERT_EQUAL_INT(getMaxOffset(st), getMaxOffset(clone_st));
    TEST_ASSERT_EQUAL_INT(getTotalSymbolAmount(st), getTotalSymbolAmount(clone_st));

    Symbol* clone_n = lookupVar(clone_st, "n");
    TEST_ASSERT_NOT_NULL(clone_n);
    TEST_ASSERT_TRUE(src_n != clone_n);
    Symbol* clone_m = lookupVar(clone_st, "m");
    TEST_ASSERT_NOT_NULL(clone_m);
    TEST_ASSERT_TRUE(src_m != clone_m);
    Symbol* clone_k = lookupVar(clone_st, "k");
    TEST_ASSERT_NOT_NULL(clone_k);
    TEST_ASSERT_TRUE(src_k != clone_k);
}

void cloneTableWithConcurrentScopesReturnsIdenticalTable() {
    st = newSymbolTable(TABLE_CAPACITY, SCOPE_CAPACITY);
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_n = res.result_value;
    Scope* src_m_scope = enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_m = res.result_value;
    TEST_ASSERT_TRUE(leaveScope(st));
    enterScope(st, SCOPE_CAPACITY);
    res = defineVar(st, AST_TYPE_INT, "k", false);
    TEST_ASSERT_TRUE(isOK(res));
    Symbol* src_k = res.result_value;

    SymbolTable* clone_st = newSymbolTableClone(st);
    TEST_ASSERT_NOT_NULL(clone_st);
    TEST_ASSERT_EQUAL_INT(getMaxOffset(st), getMaxOffset(clone_st));
    TEST_ASSERT_EQUAL_INT(getTotalSymbolAmount(st), getTotalSymbolAmount(clone_st));

    Symbol* clone_n = lookupVar(clone_st, "n");
    TEST_ASSERT_NOT_NULL(clone_n);
    TEST_ASSERT_TRUE(src_n != clone_n);
    Scope* clone_m_scope = getScope(clone_st, getScopeIndex(src_m_scope));
    Symbol* clone_m = lookupVarInScope(clone_m_scope, "m");
    TEST_ASSERT_NOT_NULL(clone_m);
    TEST_ASSERT_TRUE(src_m != clone_m);
    Symbol* clone_k = lookupVar(clone_st, "k");
    TEST_ASSERT_NOT_NULL(clone_k);
    TEST_ASSERT_TRUE(src_k != clone_k);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(newSymbolTableReturnsValidEmptyTable);
    RUN_TEST(deleteSymbolTableSetsVarNull);
    RUN_TEST(newVarHasZeroRedefLevel);
    RUN_TEST(variablesAreRetrievableAfterResizeOfScope);
    RUN_TEST(variablesAreRetrievableAfterResizeOfTable);
    RUN_TEST(defineVarIncreasesMaxOffsetAndTotalSymbolAmount);
    RUN_TEST(lookupDefinedVarReturnsVar);
    RUN_TEST(lookupVarDefinedInParentScopeReturnsVar);
    RUN_TEST(lookupVarDefinedInChildScopeReturnsErr);
    RUN_TEST(redefineVarInSameScopeWithoutRedefReturnsErr);
    RUN_TEST(redefineVarInSameScopeWithRedefReturnsErr);
    RUN_TEST(defineNewVarWithRedefReturnsErr);
    RUN_TEST(redefinedVarInChildScopeWithoutRedefReturnsErr);
    RUN_TEST(redefineVarInChildScopeWithRedefRetursOk);
    RUN_TEST(redefinedVarsInNestedScopesHaveHigherRedefLevel);
    RUN_TEST(redefinedVarsInConcurrentScopesHaveSameRedefLevel);
    RUN_TEST(leaveRootScopeFails);
    RUN_TEST(getInvalidScopeIndexReturnsErr);
    RUN_TEST(getLeftReferenceOfUndefinedVarReturnsErr);
    RUN_TEST(getLeftReferenceOfDefinedVarReturnsOk);
    RUN_TEST(getRightReferenceOfUndefinedVarReturnsErr);
    //RUN_TEST(getRightReferenceOfDefinedUninitializedVarReturnsErr);
    RUN_TEST(getRightReferenceOfDefinedInitializedVarReturnsOk);
    RUN_TEST(offsetOfVarsInNestedScopesIncreases);
    RUN_TEST(offsetOfVarsInConcurrentScopesIsEqual);
    RUN_TEST(cloneTableWithConcurrentScopesReturnsIdenticalTable);
    RUN_TEST(cloneTableWithNestedScopesReturnsIdenticalTable);
    RUN_TEST(cloneTableReturnsIdenticalTable);
    RUN_TEST(cloneEmptyTableReturnsNewEmptyTable);
    return UNITY_END();
}