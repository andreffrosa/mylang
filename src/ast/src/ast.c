#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "ast.h"

typedef union TypeHandler {
    ASTResult (*unary)(ASTType child_type);
    ASTResult (*binary)(ASTType left_type, ASTType right_type);
} TypeHandler;

typedef struct ASTNodeInfo {
    ASTOpType op_type;
    bool is_stmt;
    TypeHandler type_handler;
} ASTNodeInfo;

ASTResult binaryExpressionTypeHandler(ASTType left_type, ASTType right_type) {
    if(left_type != AST_TYPE_INT) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(right_type != AST_TYPE_INT) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    return OK(AST_TYPE_INT);
}

ASTResult binaryBooleanExpressionTypeHandler(ASTType left_type, ASTType right_type) {
    if(left_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(right_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    return OK(AST_TYPE_BOOL);
}

ASTResult binaryBitwiseExpressionTypeHandler(ASTType left_type, ASTType right_type) {
    if(left_type != AST_TYPE_INT && left_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(right_type != AST_TYPE_INT && right_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if(left_type != right_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    return OK(left_type);
}

ASTResult unaryBitwiseExpressionTypeHandler(ASTType child_type) {
    if(child_type != AST_TYPE_INT && child_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    return OK(child_type);
}

ASTResult unaryExpressionTypeHandler(ASTType child_type) {
    if(child_type != AST_TYPE_INT) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    return OK(AST_TYPE_INT);
}

ASTResult unaryBooleanExpressionTypeHandler(ASTType child_type) {
    if(child_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    return OK(AST_TYPE_BOOL);
}

ASTResult declAssignmentTypeHandler(ASTType left_type, ASTType right_type) {
    if(right_type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if(left_type != right_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    return OK(AST_TYPE_VOID);
}

ASTResult assignmentTypeHandler(ASTType left_type, ASTType right_type) {
    if(right_type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if(left_type != right_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    return OK(left_type);
}


ASTResult genericStatementTypeHandler() {
    return OK(AST_TYPE_VOID);
}

// Lookup Table
ASTNodeInfo ASTNodeTable[] = {
    [AST_INT] = {ZEROARY_OP, false, {NULL}},
    [AST_BOOL] = {ZEROARY_OP, false, {NULL}},
    [AST_ADD] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_SUB] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_MUL] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_DIV] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_MOD] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_USUB] = {UNARY_OP, false, .type_handler.unary = &unaryExpressionTypeHandler},
    [AST_UADD] = {UNARY_OP, false, .type_handler.unary = &unaryExpressionTypeHandler},
    [AST_BITWISE_OR] = {BINARY_OP, false, .type_handler.binary = &binaryBitwiseExpressionTypeHandler},
    [AST_BITWISE_AND] = {BINARY_OP, false, .type_handler.binary = &binaryBitwiseExpressionTypeHandler},
    [AST_BITWISE_XOR] = {BINARY_OP, false, .type_handler.binary = &binaryBitwiseExpressionTypeHandler},
    [AST_BITWISE_NOT] = {UNARY_OP, false, .type_handler.unary = &unaryBitwiseExpressionTypeHandler},
    [AST_L_SHIFT] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_R_SHIFT] = {BINARY_OP, false, .type_handler.binary = &binaryExpressionTypeHandler},
    [AST_ABS] = {UNARY_OP, false, .type_handler.unary = &unaryExpressionTypeHandler},
    [AST_SET_POSITIVE] = {UNARY_OP, false, .type_handler.unary = &unaryExpressionTypeHandler},
    [AST_SET_NEGATIVE] = {UNARY_OP, false, .type_handler.unary = &unaryExpressionTypeHandler},
    [AST_LOGICAL_NOT] = {UNARY_OP, false, .type_handler.unary = &unaryBooleanExpressionTypeHandler},
    [AST_LOGICAL_AND] = {BINARY_OP, false, .type_handler.binary = &binaryBooleanExpressionTypeHandler},
    [AST_LOGICAL_OR] = {BINARY_OP, false, .type_handler.binary = &binaryBooleanExpressionTypeHandler},
    [AST_ID] = {ZEROARY_OP, false, {NULL}},
    [AST_ID_DECLARATION] = {UNARY_OP, true, .type_handler.unary = &genericStatementTypeHandler},
    [AST_ID_DECL_ASSIGN] = {BINARY_OP, true, .type_handler.binary = &declAssignmentTypeHandler},
    [AST_ID_ASSIGNMENT] = {BINARY_OP, true, .type_handler.binary = &assignmentTypeHandler},
    [AST_STATEMENT_SEQ] = {BINARY_OP, true, .type_handler.binary = &genericStatementTypeHandler},
    [AST_PRINT] = {UNARY_OP, true, .type_handler.unary = &genericStatementTypeHandler},
    [AST_PRINT_VAR] = {UNARY_OP, true, .type_handler.unary = &genericStatementTypeHandler},
    [AST_NO_OP] = {ZEROARY_OP, true, {NULL}},
};

ASTOpType getNodeOpType(ASTNodeType node_type) {
    return node_type >= AST_NODE_TYPES_COUNT ? UNKNOWN_OP : ASTNodeTable[node_type].op_type;
}

static inline ASTNode* newASTNode(const ASTNodeType node_type, const ASTType value_type, const unsigned int size) {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->node_type = node_type;
    node->value_type = value_type;
    node->size = size;
    return node;
}

ASTNode* newASTInt(const int n) {
    ASTNode* node = newASTNode(AST_INT, AST_TYPE_INT, 1);
    node->n = n;
    return node;
}

ASTNode* newASTBool(const bool z) {
    ASTNode* node = newASTNode(AST_BOOL, AST_TYPE_BOOL, 1);
    node->z = z;
    return node;
}

ASTResult newASTBinaryOP(const ASTNodeType node_type, const ASTNode* left, const ASTNode* right) {
    assert(left != NULL && right != NULL);

    ASTResult res = ASTNodeTable[node_type].type_handler.binary(left->value_type, right->value_type);
    if(isERR(res)) {
        return res;
    }
    ASTType value_type = (ASTType) res.result_value;

    ASTNode* node = newASTNode(node_type, value_type, left->size + right->size + 1);
    node->left = (ASTNode*) left;
    node->right = (ASTNode*) right;
    return OK(node);
}

ASTResult newASTUnaryOP(const ASTNodeType node_type, const ASTNode* child) {
    assert(child != NULL);

    ASTResult res = ASTNodeTable[node_type].type_handler.unary(child->value_type);
    if(isERR(res)) {
        return res;
    }
    ASTType value_type = (ASTType) res.result_value;

    ASTNode* node = newASTNode(node_type, value_type, child->size + 1);
    node->child = (ASTNode*) child;
    return OK(node);
}

ASTNode* newASTID(const Symbol* id) {
    assert(id != NULL);

    ASTNode* node = newASTNode(AST_ID, getVarType(id), 1);
    node->id = (Symbol*) id;
    return node;
}

ASTResult newASTIDDefinition(const ASTType type, const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    if(!checkIDWasNotDeclared(id, st)) {
        return ERR(AST_RES_ERR_ID_ALREADY_DEFINED);
    }
    Symbol* var = insertVar(st, type, id);
    return OK(newASTID(var));
}

ASTResult newASTIDReference(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    Symbol* var = lookupVar(st, id);
    if(var == NULL) {
        return ERR(AST_RES_ERR_ID_NOT_DEFINED);
    }

    if(!isVarInitialized(var)) {
        return ERR(AST_RES_ERR_ID_NOT_INIT);
    }

    return OK(newASTID(var));
}

ASTResult newASTIDLeftReference(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    Symbol* var = lookupVar(st, id);
    if(var == NULL) {
        return ERR(AST_RES_ERR_ID_NOT_DEFINED);
    }
    return OK(newASTID(var));
}

ASTResult newASTIDDeclaration(const ASTType type, const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    if(type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    ASTResult res = newASTIDDefinition(type, id, st);
    if(isERR(res)) {
        return res;
    }
    ASTNode* ast = res.result_value;

    res = newASTUnaryOP(AST_ID_DECLARATION, ast);
    if(isERR(res)) {
        deleteASTNode(&ast);
    }
    return res;
}

ASTResult newASTIDDeclarationAssignment(const ASTType type, const char* id, const ASTNode* value, SymbolTable* st) {
    assert(id != NULL && value != NULL && st != NULL);

    if(type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    ASTResult res = newASTIDDefinition(type, id, st);
    if(isERR(res)) {
        return res;
    }

    ASTNode* ast = res.result_value;
    Symbol* var = ast->id;
    setVarInitialized(var);

    res = newASTBinaryOP(AST_ID_DECL_ASSIGN, res.result_value, value);
    if(isERR(res)) {
        deleteASTNode(&ast);
    }
    return res;
}

ASTResult newASTAssignment(const char* id, const ASTNode* value, SymbolTable* st) {
    assert(id != NULL && value != NULL && st != NULL);

    ASTResult res = newASTIDLeftReference(id, st);
    if(isERR(res)) {
        return res;
    }

    ASTNode* ast = res.result_value;
    Symbol* var = ast->id;
    setVarInitialized(var);
    res = newASTBinaryOP(AST_ID_ASSIGNMENT, res.result_value, value);
    if(isERR(res)) {
        deleteASTNode(&ast);
    }
    return res;
}

ASTNode* newASTNoOp() {
    return newASTNode(AST_NO_OP, AST_TYPE_VOID, 1);
}

void deleteASTNode(ASTNode** node) {
    assert(node != NULL);
    switch(getNodeOpType((*node)->node_type)) {
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

    if(ast1->size != ast2->size || ast1->node_type != ast2->node_type || ast1->value_type != ast2->value_type) {
        return false;
    }

    switch(getNodeOpType(ast1->node_type)) {
        case ZEROARY_OP: {
            switch (ast1->node_type) {
                case AST_INT:
                    return ast1->n == ast2->n;
                case AST_BOOL:
                    return ast1->z == ast2->z;
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
    return ASTNodeTable[ast->node_type].is_stmt;
}

bool isExp(const ASTNode* ast) {
    return !ASTNodeTable[ast->node_type].is_stmt;
}