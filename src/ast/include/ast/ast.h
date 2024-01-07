#ifndef _AST_H_
#define _AST_H_

typedef enum ASTNodeType {
    AST_NUMBER,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_MOD,
    AST_USUB,
    AST_UADD,
    AST_ABS,
    AST_SET_POSITIVE,
    AST_SET_NEGATIVE,
    AST_NODE_TYPES_COUNT  // Count of AST node types
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    unsigned int size;
    union {
        int n; // AST_NUMBER
        struct { // AST_ADD, AST_SUB, AST_MUL
            struct ASTNode* left;
            struct ASTNode* right;
        };
        struct {
            struct ASTNode* child;
        };
    };
} ASTNode;

void deleteASTNode(ASTNode* node);

ASTNode* newASTNumber(int n);

ASTNode* newASTBinaryOP(ASTNodeType type, ASTNode* left, ASTNode* right);

ASTNode* newASTUnaryOP(ASTNodeType type, ASTNode* child);

int evalAST(ASTNode* node);

#endif