#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "ast.h"

#define getNodeOPType(node) (ASTNodeTable[node->node_type].op_type)
#define isBinaryOP(node) (getNodeOPType(node) == BINARY_OP)

typedef struct ASTNodeInfo {
    const char* str;
    ASTOpType op_type;
    bool is_stmt;
    ASTResult (*type_handler)(ASTNode* node);
} ASTNodeInfo;


ASTResult binaryExpressionTypeHandler(ASTNode* node) {
    if(node->left->value_type != AST_TYPE_INT) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(node->right->value_type != AST_TYPE_INT) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    return OK(AST_TYPE_INT);
}

ASTResult binaryBooleanExpressionTypeHandler(ASTNode* node) {
    if(node->left->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(node->right->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    return OK(AST_TYPE_BOOL);
}

ASTResult unaryBooleanExpressionTypeHandler(ASTNode* node) {
    if(node->child->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    return OK(AST_TYPE_BOOL);
}

ASTResult binaryBitwiseExpressionTypeHandler(ASTNode* node) {
    if(node->left->value_type != AST_TYPE_INT && node->left->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(node->right->value_type != AST_TYPE_INT && node->right->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if(node->left->value_type != node->right->value_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    return OK(node->left->value_type);
}

ASTResult unaryBitwiseExpressionTypeHandler(ASTNode* node) {
    if(node->child->value_type != AST_TYPE_INT && node->child->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    return OK(node->child->value_type);
}

ASTResult unaryExpressionTypeHandler(ASTNode* node) {
    if(node->child->value_type != AST_TYPE_INT) {
        return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
    }

    return OK(AST_TYPE_INT);
}

ASTResult declAssignmentTypeHandler(ASTNode* node) {
    if(node->right->value_type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if(node->left->value_type != node->right->value_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    return OK(AST_TYPE_VOID);
}

ASTResult assignmentTypeHandler(ASTNode* node) {
    if(node->right->value_type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if(node->left->value_type != node->right->value_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    return OK(node->left->value_type);
}

ASTResult anyUnaryExpressionTypeHandler(ASTNode* node) {
        return OK(node->child->value_type);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
ASTResult genericStatementTypeHandler(ASTNode* node) {
    return OK(AST_TYPE_VOID);
}

ASTResult typeTypeHandler(ASTNode* node) {
    return OK(AST_TYPE_TYPE);
}
#pragma GCC diagnostic pop


bool isCmpExp(const ASTNode* ast) {
    switch (ast->node_type) {
        case AST_CMP_EQ:
        case AST_CMP_NEQ:
        case AST_CMP_LT:
        case AST_CMP_LTE:
        case AST_CMP_GT:
        case AST_CMP_GTE:
            return true;
        default:
            return false;
    }
}

bool isEqCmpExp(const ASTNode* ast) {
    return ast->node_type == AST_CMP_EQ || ast->node_type == AST_CMP_NEQ;
}

bool isTypeInequal(const ASTType type) {
    return type == AST_TYPE_INT;
}

static inline bool isTransitive(const ASTNode* node) {
    if (node->node_type == AST_CMP_NEQ) {
        return false;
    }

    if (node->node_type == AST_CMP_EQ) {
        return true;
    }

    // Find the previous non-eq cmp node
    const ASTNode* current = node->left;
    while (current->node_type == AST_CMP_EQ) {
        current = current->left;
    }

    if (!isCmpExp(current)) { // No previous non-eq cmp node, therefore it is transitive
        return true;
    }

    switch (node->node_type) {
        case AST_CMP_LT:
        case AST_CMP_LTE:
            return current->node_type == AST_CMP_LT || current->node_type == AST_CMP_LTE;
        case AST_CMP_GT:
        case AST_CMP_GTE:
            return current->node_type == AST_CMP_GT || current->node_type == AST_CMP_GTE;
        default:
            return false;
    }
}

ASTResult binaryCmpExpressionTypeHandler(ASTNode* node) {
    assert(node);

    ASTType left_type = node->left->value_type;
    ASTType right_type = node->right->value_type;

    if(left_type == AST_TYPE_BOOL) {
        if(right_type == AST_TYPE_BOOL) {
            if(isEqCmpExp(node)) {
                bool isChained = isCmpExp(node->left);
                if(isChained) {
                    if(isTransitive(node)) {
                        return OK(AST_TYPE_BOOL);
                    } else {
                        return ERR(AST_RES_ERR_NON_TRANSITIVE_CHAINING);
                    }
                } else {
                    if(isCmpExp(node->right)) { // Right can never be cmp
                        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
                    } else {
                        return OK(AST_TYPE_BOOL);
                    }
                }
            } else { // Bools cannot be compared with inequality comparators
                return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
            }
        } else { // May be either Chained or Error
            bool isChained = isCmpExp(node->left);
            if(isChained) {
                if(isTransitive(node)) {
                    return OK(AST_TYPE_BOOL);
                } else {
                    return ERR(AST_RES_ERR_NON_TRANSITIVE_CHAINING);
                }
            } else {
                // Since it is not chained, then it is an error.
                // However, since left is bool, the operator can only be equality.
                // This error should have priority over the diff types
                if(!isEqCmpExp(node)) {
                    return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
                } else {
                    return ERR(AST_RES_ERR_DIFFERENT_TYPES);
                }
            }
        }
    } else {
        bool isEq = isEqCmpExp(node);
        if(isEq || isTypeInequal(left_type)) {
            if(isEq || isTypeInequal(right_type)) {
                if(left_type != right_type) {
                    return ERR(AST_RES_ERR_DIFFERENT_TYPES);
                } else {
                    return OK(AST_TYPE_BOOL);
                }
            } else {
                return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
            }
        } else {
            return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
        }
    }
}

// Lookup Table
ASTNodeInfo ASTNodeTable[] = {
    [AST_INT]           = {"AST_INT",            ZEROARY_OP,    false, NULL},
    [AST_BOOL]          = {"AST_BOOL",           ZEROARY_OP,    false, NULL},
    [AST_ADD]           = {"AST_ADD",            BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_SUB]           = {"AST_SUB",            BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_MUL]           = {"AST_MUL",            BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_DIV]           = {"AST_DIV",            BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_MOD]           = {"AST_MOD",            BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_USUB]          = {"AST_USUB",           UNARY_OP,      false, &unaryExpressionTypeHandler},
    [AST_UADD]          = {"AST_UADD",           UNARY_OP,      false, &unaryExpressionTypeHandler},
    [AST_BITWISE_OR]    = {"AST_BITWISE_OR",     BINARY_OP,     false, &binaryBitwiseExpressionTypeHandler},
    [AST_BITWISE_AND]   = {"AST_BITWISE_AND",    BINARY_OP,     false, &binaryBitwiseExpressionTypeHandler},
    [AST_BITWISE_XOR]   = {"AST_BITWISE_XOR",    BINARY_OP,     false, &binaryBitwiseExpressionTypeHandler},
    [AST_BITWISE_NOT]   = {"AST_BITWISE_NOT",    UNARY_OP,      false, &unaryBitwiseExpressionTypeHandler},
    [AST_L_SHIFT]       = {"AST_L_SHIFT",        BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_R_SHIFT]       = {"AST_R_SHIFT",        BINARY_OP,     false, &binaryExpressionTypeHandler},
    [AST_ABS]           = {"AST_ABS",            UNARY_OP,      false, &unaryExpressionTypeHandler},
    [AST_SET_POSITIVE]  = {"AST_SET_POSITIVE",   UNARY_OP,      false, &unaryExpressionTypeHandler},
    [AST_SET_NEGATIVE]  = {"AST_SET_NEGATIVE",   UNARY_OP,      false, &unaryExpressionTypeHandler},
    [AST_LOGICAL_NOT]   = {"AST_LOGICAL_NOT",    UNARY_OP,      false, &unaryBooleanExpressionTypeHandler},
    [AST_LOGICAL_AND]   = {"AST_LOGICAL_AND",    BINARY_OP,     false, &binaryBooleanExpressionTypeHandler},
    [AST_LOGICAL_OR]    = {"AST_LOGICAL_OR",     BINARY_OP,     false, &binaryBooleanExpressionTypeHandler},
    [AST_ID]            = {"AST_ID",             ZEROARY_OP,    false, NULL},
    [AST_ID_DECLARATION]= {"AST_ID_DECLARATION", UNARY_OP,      true,  &genericStatementTypeHandler},
    [AST_ID_DECL_ASSIGN]= {"AST_ID_DECL_ASSIGN", BINARY_OP,     true,  &declAssignmentTypeHandler},
    [AST_ID_ASSIGNMENT] = {"AST_ID_ASSIGNMENT",  BINARY_OP,     true,  &assignmentTypeHandler},
    [AST_STATEMENT_SEQ] = {"AST_STATEMENT_SEQ",  BINARY_OP,     true,  &genericStatementTypeHandler},
    [AST_PRINT]         = {"AST_PRINT",          UNARY_OP,      true,  &genericStatementTypeHandler},
    [AST_PRINT_VAR]     = {"AST_PRINT_VAR",      UNARY_OP,      true,  &genericStatementTypeHandler},
    [AST_NO_OP]         = {"AST_NO_OP",          ZEROARY_OP,    true,  NULL},
    [AST_TYPE]          = {"AST_TYPE",           ZEROARY_OP,    false, NULL},
    [AST_TYPE_OF]       = {"AST_TYPE_OF",        UNARY_OP,      false, &typeTypeHandler},
    [AST_PARENTHESES]   = {"AST_PARENTHESES",    UNARY_OP,      false, &anyUnaryExpressionTypeHandler},
    [AST_CMP_EQ]        = {"AST_CMP_EQ",         BINARY_OP,     false, &binaryCmpExpressionTypeHandler},
    [AST_CMP_NEQ]       = {"AST_CMP_NEQ",        BINARY_OP,     false, &binaryCmpExpressionTypeHandler},
    [AST_CMP_LT]        = {"AST_CMP_LT",         BINARY_OP,     false, &binaryCmpExpressionTypeHandler},
    [AST_CMP_LTE]       = {"AST_CMP_LTE",        BINARY_OP,     false, &binaryCmpExpressionTypeHandler},
    [AST_CMP_GT]        = {"AST_CMP_GT",         BINARY_OP,     false, &binaryCmpExpressionTypeHandler},
    [AST_CMP_GTE]       = {"AST_CMP_GTE",        BINARY_OP,     false, &binaryCmpExpressionTypeHandler},
    [AST_SCOPE]         = {"AST_SCOPE",          UNARY_OP,      true,  &genericStatementTypeHandler},
};

ASTOpType getNodeOpType(ASTNodeType node_type) {
    return node_type >= AST_NODE_TYPES_COUNT ? UNKNOWN_OP : ASTNodeTable[node_type].op_type;
}

const char* nodeTypeToStr(ASTNodeType node_type) {
    return node_type >= AST_NODE_TYPES_COUNT ? "Unknow" : ASTNodeTable[node_type].str;
}

static inline ASTNode* newASTNode(const ASTNodeType node_type, const unsigned int size) {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->node_type = node_type;
    node->value_type = AST_TYPE_COUNT;
    node->size = size;
    return node;
}

ASTNode* newASTInt(const int n) {
    ASTNode* node = newASTNode(AST_INT, 1);
    node->value_type = AST_TYPE_INT;
    node->n = n;
    return node;
}

ASTNode* newASTBool(const bool z) {
    ASTNode* node = newASTNode(AST_BOOL, 1);
    node->value_type = AST_TYPE_BOOL;
    node->z = z;
    return node;
}

ASTNode* newASTType(const ASTType t) {
    ASTNode* node = newASTNode(AST_TYPE, 1);
    node->value_type = AST_TYPE_TYPE;
    node->t = t;
    return node;
}

ASTResult newASTBinaryOP(const ASTNodeType node_type, const ASTNode* left, const ASTNode* right) {
    assert(left != NULL && right != NULL);

    ASTNode* node = newASTNode(node_type, left->size + right->size + 1);
    node->left = left;
    node->right = right;

    ASTResult res = ASTNodeTable[node_type].type_handler(node);
    if(isERR(res)) {
        return ERR_VAL(res.result_type, node);
    }
    node->value_type = (ASTType) res.result_value;

    return OK(node);
}

ASTResult newASTUnaryOP(const ASTNodeType node_type, const ASTNode* child) {
    assert(child != NULL);

    ASTNode* node = newASTNode(node_type, child->size + 1);
    node->child = child;

    ASTResult res = ASTNodeTable[node_type].type_handler(node);
    if(isERR(res)) {
        return ERR_VAL(res.result_type, node);
    }
    node->value_type = (ASTType) res.result_value;

    return OK(node);
}

ASTNode* newASTID(Symbol* id) {
    assert(id != NULL);

    ASTNode* node = newASTNode(AST_ID, 1);
    node->value_type = getVarType(id);
    node->id = id;
    return node;
}

ASTResult newASTIDReference(const char* id, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    ASTResult res = getVarReference(st, id, false);
    if (isERR(res)) {
        return res;
    }
    Symbol* var = res.result_value;
    return OK(newASTID(var));
}

ASTResult newASTIDDeclaration(ASTType type, const char* id, const ASTNode* value, bool redef, SymbolTable* st) {
    assert(id != NULL && st != NULL);

    if (type == AST_TYPE_VOID) {
        return ERR_VAL(AST_RES_ERR_INVALID_TYPE, ASTTypeToStr(type));
    }

    bool is_init = value != NULL;

    ASTResult res = defineVar(st, type, id, is_init, redef);
    if (isERR(res)) {
        return res;
    }
    Symbol* var = res.result_value;

    if (is_init) {
        return newASTBinaryOP(AST_ID_DECL_ASSIGN, newASTID(var), value);
    } else {
        return newASTUnaryOP(AST_ID_DECLARATION, newASTID(var));
    }
}

ASTResult newASTAssignment(const char* id, const ASTNode* value, SymbolTable* st) {
    assert(id != NULL && value != NULL && st != NULL);

    ASTResult res = getVarReference(st, id, true);
    if (isERR(res)) {
        return res;
    }

    Symbol* var = res.result_value;
    setVarInitialized(var);

    return newASTBinaryOP(AST_ID_ASSIGNMENT, newASTID(var), value);
}

ASTNode* newASTNoOp() {
    ASTNode* node = newASTNode(AST_NO_OP, 1);
    node->value_type = AST_TYPE_VOID;
    return node;
}

void deleteASTNode(ASTNode** node) {
    assert(node != NULL && *node != NULL);
    switch(getNodeOpType((*node)->node_type)) {
        case ZEROARY_OP:
            break;
        case BINARY_OP:
            deleteASTNode((ASTNode**)&((*node)->left));
            deleteASTNode((ASTNode**)&((*node)->right));
            break;
        case UNARY_OP:
            deleteASTNode((ASTNode**)&((*node)->child));
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
                case AST_TYPE:
                    return ast1->t == ast2->t;
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

bool isRestrainedExp(const ASTNode* ast) {
    return ast->node_type == AST_ID_ASSIGNMENT;
}