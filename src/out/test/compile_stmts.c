#include <unity.h>

#include "ast/ast.h"
#include "out/out.h"

#include "test_utils.h"

static SymbolTable* st = NULL;
static ASTNode* ast = NULL;

void setUp (void) {
    st = newSymbolTable(1, 1);
}

void tearDown (void) {
    deleteSymbolTable(&st);
    deleteASTNode(&ast);
}

void testVarDeclaration() {
    ast = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n;\n");
}

void testVarDeclarationAssignment() {
    ast = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(100), false, st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n = 100;\n");
}

void testAssignment() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTNode* id = newASTIDReference("n", st).result_value;
    ast = newASTAssignment(id, newASTInt(100)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "n = 100;\n");
}

void testIDReference() {
    defineVar(st, AST_TYPE_INT, "n", false);
    ASTNode* id_node = newASTIDReference("n", st).result_value;
    ast = newASTAdd(id_node, newASTInt(1)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, "n + 1");
}

void testStatementSequence() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASTNode* id_node = newASTIDReference("n", st).result_value;
    ASTNode* stmt2 = newASTAssignment(id_node, newASTInt(100)).result_value;
    ast = newASTStatementList(stmt1, stmt2);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n;\nn = 100;\n");
}

void testPrintC() {
    ast = newASTPrint(newASTInt(100));
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", 100);\n");
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", 1 + 1);\n");
    deleteASTNode(&ast);

    defineVar(st, AST_TYPE_INT, "n", false);
    ast = newASTPrint(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", n);\n");
    deleteASTNode(&ast);

    ast = newASTPrintVar(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"n = %d\\n\", n);\n");
}

void testBoolPrintC() {
    ast = newASTPrint(newASTBool(true));
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", (true ? \"true\" : \"false\"));\n");
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", (true && false ? \"true\" : \"false\"));\n");
    deleteASTNode(&ast);

    defineVar(st, AST_TYPE_BOOL, "z", false);
    ast = newASTPrint(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", (z ? \"true\" : \"false\"));\n");
    deleteASTNode(&ast);

    ast = newASTPrintVar(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"z = %s\\n\", (z ? \"true\" : \"false\"));\n");
}

void testPrintJava() {
    ast = newASTPrint(newASTInt(100));
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(100);\n");
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(1 + 1);\n");
    deleteASTNode(&ast);

    defineVar(st, AST_TYPE_INT, "n", false);
    ast = newASTPrint(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(n);\n");
    deleteASTNode(&ast);

    ast = newASTPrintVar(newASTIDReference("n", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(\"n = \" + n);\n");
}

void testBoolPrintJava() {
    ast = newASTPrint(newASTBool(true));
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(true);\n");
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(true && false);\n");
    deleteASTNode(&ast);

    defineVar(st, AST_TYPE_BOOL, "z", false);
    ast = newASTPrint(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(z);\n");
    deleteASTNode(&ast);

    ast = newASTPrintVar(newASTIDReference("z", st).result_value);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(\"z = \" + z);\n");
}

void compileRestrainedExpression() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st).result_value;
    ASTNode* id_node = newASTIDReference("n", st).result_value;
    ASTNode* value_node = newASTAdd(id_node, newASTInt(1)).result_value;
    id_node = newASTIDReference("n", st).result_value;
    ASTNode* restr_exp = newASTAssignment(id_node, value_node).result_value; // valueof(n=n+1)
    ASTNode* stmt2 = newASTIDDeclaration(AST_TYPE_INT, "m", restr_exp, false, st).result_value;
    ast = newASTStatementList(stmt1, stmt2);

    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int n = 0;\nint m = (n = n + 1);\n");
}

void compileBoolVarsInC() {
    ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", NULL, false, st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "bool z;\n");
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
    st = newSymbolTable(1, 1);

    ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(true), false, st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "bool z = true;\n");
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
    st = newSymbolTable(1, 1);

    defineVar(st, AST_TYPE_BOOL, "z", false);
    ASTNode* id_node = newASTIDReference("z", st).result_value;
    ast = newASTAssignment(id_node, newASTBool(true)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "z = true;\n");
}

void compileBoolVarsInJava() {
    ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", NULL, false, st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "boolean z;\n");
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
    st = newSymbolTable(1, 1);

    ast = newASTIDDeclaration(AST_TYPE_BOOL, "z", newASTBool(true), false, st).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "boolean z = true;\n");
    deleteASTNode(&ast);
    deleteSymbolTable(&st);
    st = newSymbolTable(1, 1);

    defineVar(st, AST_TYPE_BOOL, "z", false);
    ASTNode* id_node = newASTIDReference("z", st).result_value;
    ast = newASTAssignment(id_node, newASTBool(true)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "z = true;\n");
}

static ASTNode* getRedefAST() {
    ASTResult res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(0), false, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* decl = res.result_value;

    enterScope(st, 1);
    res = newASTIDDeclaration(AST_TYPE_INT, "n", newASTInt(1), true, st);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* scope = newASTScope(res.result_value);
    leaveScope(st);

    return newASTStatementList(decl, scope);
}

void compileRedefVarInCDoesNotInsertRedefLevel() {
    ast = getRedefAST();
    const char* str = "int n = 0;\n{\n    int n = 1;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, str);
}

void compileRedefVarInJavaInsertsRedefLevel() {
    ast = getRedefAST();
    const char* str = "int n = 0;\n{\n    int n_1_ = 1;\n}\n";
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, str);
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
    RUN_TEST(compileRedefVarInCDoesNotInsertRedefLevel);
    RUN_TEST(compileRedefVarInJavaInsertsRedefLevel);
    return UNITY_END();
}