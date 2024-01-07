#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "ast.h"

typedef enum ASTOpType {
    NUMBER_OP,
    UNARY_OP,
    BINARY_OP,
    UNKNOWN_OP
} ASTOpType;

// Lookup table
ASTOpType typeMappings[] = {
    [AST_NUMBER] = NUMBER_OP,
    [AST_ADD] = BINARY_OP,
    [AST_SUB] = BINARY_OP,
    [AST_MUL] = BINARY_OP,
    [AST_DIV] = BINARY_OP,
    [AST_MOD] = BINARY_OP,
    [AST_USUB] = UNARY_OP,
    [AST_UADD] = UNARY_OP,
    [AST_ABS] = UNARY_OP,
    [AST_SET_POSITIVE] = UNARY_OP,
    [AST_SET_NEGATIVE] = UNARY_OP,
};

#define getNodeOpType(nodeType) (nodeType >= AST_NODE_TYPES_COUNT ? UNKNOWN_OP : typeMappings[nodeType])

static inline ASTNode* newASTNode(ASTNodeType type) {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* newASTNumber(int n) {
    ASTNode* node = newASTNode(AST_NUMBER);
    node->n = n;
    node->size = 1;
    return node;
}

ASTNode* newASTBinaryOP(ASTNodeType type, ASTNode* left, ASTNode* right) {
    ASTNode* node = newASTNode(type);
    node->left = left;
    node->right = right;
    node->size = left->size + right->size + 1;
    return node;
}

ASTNode* newASTUnaryOP(ASTNodeType type, ASTNode* child) {
    ASTNode* node = newASTNode(type);
    node->child = child;
    node->size = child->size + 1;
    return node;
}

int evalAST(ASTNode* node) {
    assert(node != NULL);
    switch (node->type) {
        case AST_NUMBER:
            return node->n;
        case AST_ADD:
            return evalAST(node->left) + evalAST(node->right);
        case AST_SUB:
            return evalAST(node->left) - evalAST(node->right);
        case AST_MUL:
            return evalAST(node->left) * evalAST(node->right);
        case AST_DIV:
            return evalAST(node->left) / evalAST(node->right);
        case AST_MOD:
            return evalAST(node->left) % evalAST(node->right);
        case AST_USUB:
            return - evalAST(node->child);
        case AST_UADD:
            return + evalAST(node->child);
        default:
            assert(false);
    }
}

void deleteASTNode(ASTNode* node) {
    assert(node != NULL);
    switch(getNodeOpType(node->type)) {
        case NUMBER_OP:
            free(node);
            return;
        case BINARY_OP:
            deleteASTNode(node->left);
            deleteASTNode(node->right);
            free(node);
            return;
        case UNARY_OP:
            deleteASTNode(node->child);
            free(node);
            return;
        default:
            free(node);
            assert(false);
    }
}
