#include <unity.h>

#include "ast/ast.h"

#include "in.h"

#include "parser.h"

#include "test_utils.h"

SymbolTable* st = NULL;

void setUp (void) {}

void tearDown (void) {}

void parseWhileLoop() {
    ASTNode* ast = newASTWhile(newASTBool(true), newASTNoOp()).result_value;
    ASSERT_MATCH_AST("while (true) {}", ast, false);
}

void parseDoWhileLoop() {
    ASTNode* ast = newASTDoWhile(newASTNoOp(), newASTBool(true)).result_value;
    ASSERT_MATCH_AST("do {} while (true)", ast, false);
}

void parseForLoop() {
    SymbolTable* st_ = newSymbolTable(1, 1);
    ASTNode* init = newASTIDDeclaration(AST_TYPE_INT, "i", newASTInt(0), false, st_).result_value;
    ASTNode* id_node = newASTIDReference("i", st_).result_value;
    ASTNode* cond = newASTCmpLT(id_node, newASTInt(10)).result_value;
    ASTNode* update = newASTInc(copyAST(id_node), false).result_value;
    ASTNode* ast = newASTFor(init, cond, update, newASTNoOp()).result_value;
    ASSERT_MATCH_AST("for (var i = 0; i < 10; i++) {}", ast, false);
    deleteSymbolTable(&st_);
}

void parseForLoopWithoutInitNorUpdate() {
    ASTNode* ast = newASTFor(newASTNoOp(), newASTBool(true), newASTNoOp(), newASTNoOp()).result_value;
    ASSERT_MATCH_AST("for (; true; ) {}", ast, false);
}

void parseInfiniteForLoop() {
    ASTNode* ast = newASTFor(newASTNoOp(), newASTBool(true), newASTNoOp(), newASTNoOp()).result_value;
    ASSERT_MATCH_AST("for (;;) {}", ast, false);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(parseWhileLoop);
    RUN_TEST(parseDoWhileLoop);
    RUN_TEST(parseForLoop);
    RUN_TEST(parseForLoopWithoutInitNorUpdate);
    RUN_TEST(parseInfiniteForLoop);
    return UNITY_END();
}
