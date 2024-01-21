#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "ast.h"

#define OK(V) (ASTResult){.type = ASTResult_OK, .ast = V}
#define ERR(T) (ASTResult){.type = ASTResult_ERR_ ## T, .ast = NULL}

// Lookup table
ASTOpType ASTOpMap[] = {
    [AST_NUMBER] = ZEROARY_OP,
    [AST_ADD] = BINARY_OP,
    [AST_SUB] = BINARY_OP,
    [AST_MUL] = BINARY_OP,
    [AST_DIV] = BINARY_OP,
    [AST_MOD] = BINARY_OP,
    [AST_USUB] = UNARY_OP,
    [AST_UADD] = UNARY_OP,
    [AST_BITWISE_OR] = BINARY_OP,
    [AST_BITWISE_AND] = BINARY_OP,
    [AST_BITWISE_XOR] = BINARY_OP,
    [AST_BITWISE_NOT] = UNARY_OP,
    [AST_L_SHIFT] = BINARY_OP,
    [AST_R_SHIFT] = BINARY_OP,
    [AST_ABS] = UNARY_OP,
    [AST_SET_POSITIVE] = UNARY_OP,
    [AST_SET_NEGATIVE] = UNARY_OP,
    [AST_ID] = ZEROARY_OP,
    [AST_ID_DECLARATION] = UNARY_OP,
    [AST_ID_DECL_ASSIGN] = BINARY_OP,
    [AST_ID_ASSIGNMENT] = BINARY_OP,
    [AST_STATEMENT_SEQ] = BINARY_OP,
};

static inline ASTNode* newASTNode(const ASTNodeType type) {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* newASTNumber(const int n) {
    ASTNode* node = newASTNode(AST_NUMBER);
    node->n = (int) n;
    node->size = 1;
    return node;
}

ASTNode* newASTBinaryOP(const ASTNodeType type, const ASTNode* left, const ASTNode* right) {
    assert(left != NULL && right != NULL);

    ASTNode* node = newASTNode(type);
    node->left = (ASTNode*) left;
    node->right = (ASTNode*) right;
    node->size = left->size + right->size + 1;
    return node;
}

ASTNode* newASTUnaryOP(const ASTNodeType type, const ASTNode* child) {
    assert(child != NULL);

    ASTNode* node = newASTNode(type);
    node->child = (ASTNode*) child;
    node->size = child->size + 1;
    return node;
}

ASTNode* newASTID(const Symbol* id) {
    assert(id != NULL);

    ASTNode* node = newASTNode(AST_ID);
    node->id = (Symbol*) id;
    node->size = 1;
    return node;
}

ASTResult newASTIDDefinition(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    if(!checkIDWasNotDeclared(id, st)) {
        return ERR(ID_ALREADY_DEFINED);
    }
    Symbol* var = insertVar(st, id);
    return OK(newASTID(var));
}

ASTResult newASTIDReference(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    Symbol* var = lookupVar(st, id);
    if(var == NULL) {
        return ERR(ID_NOT_DEFINED);
    }

    if(!isVarInitialized(var)) {
        return ERR(ID_NOT_INIT);
    }

    return OK(newASTID(var));
}

ASTResult newASTIDLeftReference(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    Symbol* var = lookupVar(st, id);
    if(var == NULL) {
        return ERR(ID_NOT_DEFINED);
    }
    return OK(newASTID(var));
}

ASTResult newASTIDDeclaration(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    ASTResult res = newASTIDDefinition(id, st);
    if(isERR(res)) {
        return res;
    }
    return OK(newASTUnaryOP(AST_ID_DECLARATION, res.ast));
}

ASTResult newASTIDDeclarationAssignment(const char* id, const ASTNode* value, SymbolTable* st) {
    assert(id != NULL && value != NULL && st != NULL);

    ASTResult res = newASTIDDefinition(id, st);
    if(isERR(res)) {
        return res;
    }

    Symbol* var = res.ast->id;
    setVarInitialized(var);

    return OK(newASTBinaryOP(AST_ID_DECL_ASSIGN, res.ast, value));
}

ASTResult newASTAssignment(const char* id, const ASTNode* value, SymbolTable* st) {
    assert(id != NULL && value != NULL && st != NULL);

    ASTResult res = newASTIDLeftReference(id, st);
    if(isERR(res)) {
        return res;
    }

    Symbol* var = res.ast->id;
    setVarInitialized(var);

    return OK(newASTBinaryOP(AST_ID_ASSIGNMENT, res.ast, value));
}

void deleteASTNode(ASTNode** node) {
    assert(node != NULL);
    switch(getNodeOpType((*node)->type)) {
        case ZEROARY_OP:
            break;
        case BINARY_OP:
            deleteASTNode(&((*node)->left));
            deleteASTNode(&((*node)->right));
            break;
        case UNARY_OP:
            deleteASTNode(&((*node)->child));
            break;
        default:
            assert(false);
    }

    free(*node);
    *node = NULL;
}

bool equalAST(const ASTNode* ast1, const ASTNode* ast2) {
    assert(ast1 != NULL && ast2 != NULL);

    if(ast1 == ast2) {
        return true;
    }

    if(ast1->size != ast2->size || ast1->type != ast2->type) {
        return false;
    }

    switch(getNodeOpType(ast1->type)) {
        case ZEROARY_OP: {
            switch (ast1->type) {
                case AST_NUMBER:
                    return ast1->n == ast2->n;
                case AST_ID:
                    return strncmp(getVarId(ast1->id), getVarId(ast2->id), MAX_ID_SIZE) == 0;
                default:
                    assert(false);
            }
        } case UNARY_OP:
            return equalAST(ast1->child, ast2->child);
        case BINARY_OP:
            return equalAST(ast1->left, ast2->left) && equalAST(ast1->right, ast2->right);
        default:
            assert(false);
    }
}

bool isStmt(const ASTNode* ast) {
    switch (ast->type) {
    case AST_ID_DECLARATION:
    case AST_ID_DECL_ASSIGN:
    case AST_ID_ASSIGNMENT:
    case AST_STATEMENT_SEQ:
        return true;
    default:
        return false;
    }
}

bool isExp(const ASTNode* ast) {
    return !isStmt(ast);
}