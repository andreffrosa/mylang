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
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id = newASTID(res.result_value);
    ast = newASTAssignment(id, newASTInt(100)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "n = 100;\n");
}

void testIDReference() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
    ast = newASTAdd(id_node, newASTInt(1)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "n + 1");
}

void testStatementSequence() {
    ASTNode* stmt1 = newASTIDDeclaration(AST_TYPE_INT, "n", NULL, false, st).result_value;
    ASTResult res = getVarReference(st, "n");
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* id_node = newASTID(res.result_value);
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

    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ast = newASTPrint(n_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%d\\n\", n);\n");
    deleteASTNode(&ast);

    n_node = newASTID(res.result_value);
    ast = newASTPrintVar(n_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"int n = %d\\n\", n);\n");
}

void testBoolPrintC() {
    ast = newASTPrint(newASTBool(true));
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", true ? \"true\" : \"false\");\n");
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", true && false ? \"true\" : \"false\");\n");
    deleteASTNode(&ast);

    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTPrint(z_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"%s\\n\", z ? \"true\" : \"false\");\n");
    deleteASTNode(&ast);

    z_node = newASTID(res.result_value);
    ast = newASTPrintVar(z_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "printf(\"bool z = %s\\n\", z ? \"true\" : \"false\");\n");
}

void testPrintJava() {
    ast = newASTPrint(newASTInt(100));
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(100);\n");
    deleteASTNode(&ast);

    ast = newASTAdd(newASTInt(1), newASTInt(1)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(1 + 1);\n");
    deleteASTNode(&ast);

    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ast = newASTPrint(n_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(n);\n");
    deleteASTNode(&ast);

    n_node = newASTID(res.result_value);
    ast = newASTPrintVar(n_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(\"int n = \" + n);\n");
}

void testBoolPrintJava() {
    ast = newASTPrint(newASTBool(true));
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(true);\n");
    deleteASTNode(&ast);

    ast = newASTLogicalAnd(newASTBool(true), newASTBool(false)).result_value;
    ast = newASTPrint(ast);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(true && false);\n");
    deleteASTNode(&ast);

    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTPrint(z_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(z);\n");
    deleteASTNode(&ast);

    z_node = newASTID(res.result_value);
    ast = newASTPrintVar(z_node);
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "System.out.println(\"bool z = \" + z);\n");
}

void compileDeclarationWithChainedAssignment() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ASTNode* value_node = newASTAdd(n_node, newASTInt(1)).result_value;
    n_node = newASTID(res.result_value);
    ASTNode* restr_exp = newASTAssignment(n_node, value_node).result_value; // valueof(n=n+1)
    ast = newASTIDDeclaration(AST_TYPE_INT, "m", restr_exp, false, st).result_value;

    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "int m = n = n + 1;\n");
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

    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTAssignment(z_node, newASTBool(true)).result_value;
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

    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTAssignment(z_node, newASTBool(true)).result_value;
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

void compileInc() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ast = newASTInc(n_node, true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "++n");
    deleteASTNode(&ast);

    n_node = newASTID(res.result_value);
    ast = newASTInc(n_node, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "n++");
}

void compileDec() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ast = newASTDec(n_node, true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "--n");
    deleteASTNode(&ast);

    n_node = newASTID(res.result_value);
    ast = newASTDec(n_node, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "n--");
}

void compileLogicalToggle() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTLogicalToggle(z_node, true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "z = !z");
    deleteASTNode(&ast);

    z_node = newASTID(res.result_value);
    ast = newASTLogicalToggle(z_node, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "!(z = !z)");
}

void compileBitwiseToggle() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ast = newASTBitwiseToggle(n_node, true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "n = ~n");
    deleteASTNode(&ast);

    n_node = newASTID(res.result_value);
    ast = newASTBitwiseToggle(n_node, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "~(n = ~n)");
}

void compileCompoundAssignmentAdd() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    ast = newASTCompoundAssignment(AST_ADD, n_node, newASTInt(2)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "n += 2;\n");
}

void compileCompoundAssignmentLogicalAndInC() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTCompoundAssignment(AST_LOGICAL_AND, z_node, newASTBool(true)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "z &&= true;\n");
}

void compileCompoundAssignmentLogicalAndInJava() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "z", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* z_node = newASTID(res.result_value);
    ast = newASTCompoundAssignment(AST_LOGICAL_AND, z_node, newASTBool(true)).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &javaSerializer, "z = z && true");
}

void compileConditionalInc() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ast = newASTInc(copyAST(cond), true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true ? (n = (true ? n : m) + 1) : (m = (true ? n : m) + 1))");
    deleteASTNode(&ast);

    ast = newASTInc(cond, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true ? (n = (true ? n : m) + 1) : (m = (true ? n : m) + 1)) + -1");
}

void compileConditionalDec() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ast = newASTDec(copyAST(cond), true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true ? (n = (true ? n : m) + -1) : (m = (true ? n : m) + -1))");
    deleteASTNode(&ast);

    ast = newASTDec(cond, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true ? (n = (true ? n : m) + -1) : (m = (true ? n : m) + -1)) + 1");
}

void compileConditionalLogicalToggle() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_BOOL, "m", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* m_node = newASTID(res.result_value);

    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    res = newASTLogicalToggle(copyAST(cond), true);
    TEST_ASSERT_TRUE(isOK(res));
    ast = res.result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true ? (n = !(true ? n : m)) : (m = !(true ? n : m)))");
    deleteASTNode(&ast);

    ast = newASTLogicalToggle(cond, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "!(true ? (n = !(true ? n : m)) : (m = !(true ? n : m)))");
}

void compileConditionalBitwiseToggle() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_BOOL, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ASTNode* cond = newASTParentheses(newASTTernaryCond(newASTBool(true), n_node, m_node).result_value);

    ast = newASTBitwiseToggle(copyAST(cond), true).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "(true ? (n = !(true ? n : m)) : (m = !(true ? n : m)))");
    deleteASTNode(&ast);

    ast = newASTBitwiseToggle(cond, false).result_value;
    ASSERT_COMPILE_EXP_EQUALS(ast, &cSerializer, "!(true ? (n = !(true ? n : m)) : (m = !(true ? n : m)))");

}

void compileConditionalCompoundAssignmentAddInC() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;

    ast = newASTCompoundAssignment(AST_ADD, newASTParentheses(ast), newASTInt(2)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "(true ? (n += 2) : (m += 2));\n");
}

void compileConditionalCompoundAssignmentAddInJava() {
    ASTResult res = defineVar(st, AST_TYPE_INT, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_INT, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;

    ast = newASTCompoundAssignment(AST_ADD, newASTParentheses(ast), newASTInt(2)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "_tmp_int = (true ? (n += 2) : (m += 2));\n");
}

void compileConditionalCompoundAssignmentLogicalAndInC() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_BOOL, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;

    ast = newASTCompoundAssignment(AST_LOGICAL_AND, newASTParentheses(ast), newASTBool(true)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &cSerializer, "(true ? (n &&= true) : (m &&= true));\n");
}

void compileConditionalCompoundAssignmentLogicalAndInJava() {
    ASTResult res = defineVar(st, AST_TYPE_BOOL, "n", false);
    TEST_ASSERT_TRUE(isOK(res));
    ASTNode* n_node = newASTID(res.result_value);
    res = defineVar(st, AST_TYPE_BOOL, "m", false);
    ASTNode* m_node = newASTID(res.result_value);
    TEST_ASSERT_TRUE(isOK(res));

    ast = newASTTernaryCond(newASTBool(true), n_node, m_node).result_value;

    ast = newASTCompoundAssignment(AST_LOGICAL_AND, newASTParentheses(ast), newASTBool(true)).result_value;
    ASSERT_COMPILE_STMT_EQUALS(ast, &javaSerializer, "_tmp_bool = (true ? (n = (true ? n : m) && true) : (m = (true ? n : m) && true));\n");
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
    RUN_TEST(compileDeclarationWithChainedAssignment);
    RUN_TEST(compileBoolVarsInC);
    RUN_TEST(compileBoolVarsInJava);
    RUN_TEST(compileRedefVarInCDoesNotInsertRedefLevel);
    RUN_TEST(compileRedefVarInJavaInsertsRedefLevel);
    RUN_TEST(compileInc);
    RUN_TEST(compileDec);
    RUN_TEST(compileLogicalToggle);
    RUN_TEST(compileBitwiseToggle);
    RUN_TEST(compileCompoundAssignmentAdd);
    RUN_TEST(compileCompoundAssignmentLogicalAndInC);
    RUN_TEST(compileCompoundAssignmentLogicalAndInJava);
    RUN_TEST(compileConditionalInc);
    RUN_TEST(compileConditionalDec);
    RUN_TEST(compileConditionalLogicalToggle);
    RUN_TEST(compileConditionalBitwiseToggle);
    RUN_TEST(compileConditionalCompoundAssignmentAddInC);
    RUN_TEST(compileConditionalCompoundAssignmentAddInJava);
    RUN_TEST(compileConditionalCompoundAssignmentLogicalAndInC);
    RUN_TEST(compileConditionalCompoundAssignmentLogicalAndInJava);
    return UNITY_END();
}