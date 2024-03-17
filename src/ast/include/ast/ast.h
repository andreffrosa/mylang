#ifndef _AST_H_
#define _AST_H_

#include <stdbool.h>

#include "symbol.h"

typedef enum ASTNodeType {
    AST_NUMBER,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_MOD,
    AST_USUB,
    AST_UADD,
    AST_BITWISE_OR,
    AST_BITWISE_AND,
    AST_BITWISE_XOR,
    AST_BITWISE_NOT,
    AST_L_SHIFT,
    AST_R_SHIFT,
    AST_ABS,
    AST_SET_POSITIVE,
    AST_SET_NEGATIVE,
    AST_ID,
    AST_ID_DECLARATION,
    AST_ID_DECL_ASSIGN,
    AST_ID_ASSIGNMENT,
    AST_STATEMENT_SEQ,
    AST_PRINT,
    AST_PRINT_VAR,
    AST_NODE_TYPES_COUNT  // Count of AST node types
} ASTNodeType;

typedef enum ASTOpType {
    ZEROARY_OP,
    UNARY_OP,
    BINARY_OP,
    UNKNOWN_OP
} ASTOpType;

extern ASTOpType ASTOpMap[];

#define getNodeOpType(nodeType) (nodeType >= AST_NODE_TYPES_COUNT ? UNKNOWN_OP : ASTOpMap[nodeType])

typedef struct ASTNode {
    ASTNodeType type;
    unsigned int size;
    union {
        int n; // AST_NUMBER
        Symbol* id; // AST_ID
        struct { // BINARY_OP
            struct ASTNode* left;
            struct ASTNode* right;
        };
        struct { // UNARY_OP
            struct ASTNode* child;
        };
    };
} ASTNode;

typedef enum ASTResultType {
    ASTResult_OK = 0,
    ASTResult_ERR_ID_ALREADY_DEFINED,
    ASTResult_ERR_ID_NOT_DEFINED,
    ASTResult_ERR_ID_NOT_INIT,
    ASTResult_TYPE_COUNT
} ASTResultType;

typedef struct ASTResult {
    ASTResultType type;
    union {
        ASTNode* ast;
    };
} ASTResult;

#define isOK(res) (res.type == ASTResult_OK && res.ast != NULL)
#define isERR(res) !isOK(res)

void deleteASTNode(ASTNode** node);

ASTNode* newASTNumber(const int n);

ASTNode* newASTBinaryOP(const ASTNodeType type, const ASTNode* left, const ASTNode* right);

ASTNode* newASTUnaryOP(const ASTNodeType type, const ASTNode* child);

bool equalAST(const ASTNode* ast1, const ASTNode* ast2);

bool isStmt(const ASTNode* ast);

bool isExp(const ASTNode* ast);

#define newASTAdd(l, r) newASTBinaryOP(AST_ADD, l, r)
#define newASTSub(l, r) newASTBinaryOP(AST_SUB, l, r)
#define newASTMul(l, r) newASTBinaryOP(AST_MUL, l, r)
#define newASTDiv(l, r) newASTBinaryOP(AST_DIV, l, r)
#define newASTMod(l, r) newASTBinaryOP(AST_MOD, l, r)

#define newASTUSub(e) newASTUnaryOP(AST_USUB, e)
#define newASTUAdd(e) newASTUnaryOP(AST_UADD, e)

#define newASTBitwiseAnd(l, r) newASTBinaryOP(AST_BITWISE_AND, l, r)
#define newASTBitwiseOr(l, r) newASTBinaryOP(AST_BITWISE_OR, l, r)
#define newASTBitwiseXor(l, r) newASTBinaryOP(AST_BITWISE_XOR, l, r)
#define newASTBitwiseNot(e) newASTUnaryOP(AST_BITWISE_NOT, e)
#define newASTLeftShift(l, r) newASTBinaryOP(AST_L_SHIFT, l, r)
#define newASTRightShift(l, r) newASTBinaryOP(AST_R_SHIFT, l, r)

#define newASTAbs(e) newASTUnaryOP(AST_ABS, e)
#define newASTSetPositive(e) newASTUnaryOP(AST_SET_POSITIVE, e)
#define newASTSetNegative(e) newASTUnaryOP(AST_SET_NEGATIVE, e)

#define newASTStatementList(stmt, list) (list == NULL ? stmt : newASTBinaryOP(AST_STATEMENT_SEQ, stmt, list))

ASTResult newASTIDReference(const char* id, SymbolTable* st);
ASTResult newASTIDDeclaration(const char* id, SymbolTable* st);
ASTResult newASTIDDeclarationAssignment(const char* id, const ASTNode* value, SymbolTable* st);
ASTResult newASTAssignment(const char* id, const ASTNode* value, SymbolTable* st);

#define newASTPrint(e) newASTUnaryOP(AST_PRINT, e)
#define newASTPrintVar(e) newASTUnaryOP(AST_PRINT_VAR, e)

#endif