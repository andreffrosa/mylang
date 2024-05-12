#include <unity.h>

#include "ast/ast.h"
#include "out/out.h"

static SymbolTable* st;

void setUp (void) {
    st = newSymbolTable(1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
}

const char* compileStmt(const ASTNode* ast, const OutSerializer* os) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTStatements(ast, st, stream, os, 0);
    IOStreamClose(&stream);
    return ptr;
}

const char* compileExp(const ASTNode* ast) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* stream = openIOStreamFromMemmory(&ptr, &size);
    compileASTExpression(ast, st, stream, NULL);
    IOStreamClose(&stream);
    return ptr;
}

extern const OutSerializer cSerializer;
extern const OutSerializer javaSerializer;

#define ASSERT_COMPILE_STMT_EQUALS(ast, os, str) {\
    const char* txt = compileStmt(ast, os);\
    TEST_ASSERT_NOT_NULL(txt);\
    TEST_ASSERT_EQUAL_STRING(str, txt);\
    free((void*)txt);\
    deleteASTNode(&ast);\
}

#define ASSERT_COMPILE_EXP_EQUALS(ast, str) {\
    const char* txt = compileExp(ast);\
        TEST_ASSERT_NOT_NULL(txt);\
    TEST_ASSERT_EQUAL_STRING(str, txt);\
    free((void*)txt);\
    deleteASTNode(&ast);\
}

void testVarDeclaration() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_INT, "n", st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n;\n");
}

void testVarDeclarationAssignment() {
    ASTNode* ast = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(100), st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n = 100;\n");
}

void testAssignment() {
    insertVar(st, AST_TYPE_INT, "n");
    ASTNode* ast = newASTAssignment("n", newASTInt(100), st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "n = 100;\n");
}

void testIDReference() {
    Symbol* var = insertVar(st, AST_TYPE_INT, "n");
    setVarInitialized(var);
    ASTNode* id_node = newASTIDReference("n", st).result_value;
    ASTNode* ast = newASTAdd(id_node, newASTInt(1)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, "n + 1");
}

void testStatementSequence() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", st).result_value;
    ASTNode* stmt2 = newASTAssignment("n", newASTInt(100), st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n;\nn = 100;\n");
}

void testPrintC() {
    ASTNode* ast = newASTPrint(newASTInt(100));
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", 100);\n");

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", 1 + 1);\n");

    Symbol* var = insertVar(st, AST_TYPE_INT, "n");
    setVarInitialized(var);
    ast = newASTPrint(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", n);\n");

    ast = newASTPrintVar(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"n = %d\\n\", n);\n");
}

void testBoolPrintC() {
    ASTNode* ast = newASTPrint(newASTBool(true));
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", (true ? \"true\" : \"false\"));\n");

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", (true && false ? \"true\" : \"false\"));\n");

    Symbol* var = insertVar(st, AST_TYPE_BOOL, "z");
    setVarInitialized(var);
    ast = newASTPrint(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", (z ? \"true\" : \"false\"));\n");

    ast = newASTPrintVar(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"z = %s\\n\", (z ? \"true\" : \"false\"));\n");
}

void testPrintJava() {
    ASTNode* ast = newASTPrint(newASTInt(100));
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(100);\n");

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(1 + 1);\n");

    Symbol* var = insertVar(st, AST_TYPE_INT, "n");
    setVarInitialized(var);
    ast = newASTPrint(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(n);\n");

    ast = newASTPrintVar(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(\"n = \" + n);\n");
}

void testBoolPrintJava() {
    ASTNode* ast = newASTPrint(newASTBool(true));
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(true);\n");

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(true && false);\n");

    Symbol* var = insertVar(st, AST_TYPE_BOOL, "z");
    setVarInitialized(var);
    ast = newASTPrint(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(z);\n");

    ast = newASTPrintVar(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(\"z = \" + z);\n");
}

void compileRestrainedExpression() {
    ASTNode* stmt1 = newASTIDDeclarationAssignment(AST_TYPE_INT, "n", newASTInt(0), st).result_value;
    ASTNode* restr_exp = newASTAssignment("n", newASTAdd(newASTIDReference("n", st).result_value, newASTInt(1)).result_value, st).result_value; // valueof(n=n+1)
    ASTNode* stmt2 = newASTIDDeclarationAssignment(AST_TYPE_INT, "m", restr_exp, st).result_value;
    ASTNode* ast = newASTStatementList(stmt1, stmt2);

    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n = 0;\nint m = (n = n + 1);\n");
}

void compileBoolVarsInC() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "bool z;\n");

    deleteSymbolTable(&st);
    st = newSymbolTable(1);

    ast = newASTIDDeclarationAssignment(AST_TYPE_BOOL, "z", newASTBool(true), st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "bool z = true;\n");

    deleteSymbolTable(&st);
    st = newSymbolTable(1);

    insertVar(st, AST_TYPE_BOOL, "z");
    ast = newASTAssignment("z", newASTBool(true), st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "z = true;\n");
}

void compileBoolVarsInJava() {
    ASTNode* ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "boolean z;\n");

    deleteSymbolTable(&st);
    st = newSymbolTable(1);

    ast = newASTIDDeclarationAssignment(AST_TYPE_BOOL, "z", newASTBool(true), st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "boolean z = true;\n");

    deleteSymbolTable(&st);
    st = newSymbolTable(1);

    insertVar(st, AST_TYPE_BOOL, "z");
    ast = newASTAssignment("z", newASTBool(true), st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "z = true;\n");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testVarDeclaration);
    RUN_TEST(testVarDeclarationAssignment);
    RUN_TEST(testAssignment);
    RUN_TEST(testIDReference);
    RUN_TEST(testStatementSequence);
    RUN_TEST(testPrintC);
    RUN_TEST(testBoolPrintC);
    RUN_TEST(testPrintJava);
    RUN_TEST(testBoolPrintJava);
    RUN_TEST(compileRestrainedExpression);
    RUN_TEST(compileBoolVarsInC);
    RUN_TEST(compileBoolVarsInJava);
    return UNITY_END();
}