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
    ASTResult (*lval_checker)(ASTNode* node);
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
    const ASTNode* lval = node->left;
    const ASTNode* rval = node->right;

    if (rval->value_type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
    }

    if (lval->value_type != rval->value_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    /*if (node->left->node_type == AST_ID) {
        Symbol* var = node->left->id;
        setVarInitialized(var);
    }*/

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

ASTResult ternaryCondTypeHandler(ASTNode* node) {
    assert(node);

    if(node->first->value_type != AST_TYPE_BOOL) {
        return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
    }

    if(node->second->value_type != node->third->value_type) {
        return ERR(AST_RES_ERR_DIFFERENT_TYPES);
    }

    if(node->second->value_type == AST_TYPE_VOID) {
        return ERR(AST_RES_ERR_INVALID_TYPE);
    }

    return OK(node->second->value_type);
}

ASTResult ifTypeHandler(ASTNode* node) {
    assert(node != NULL);

    if (node->node_type == AST_IF || node->node_type == AST_WHILE) {
        if (node->left->value_type != AST_TYPE_BOOL) {
            return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
        }
    } else if (node->node_type == AST_IF_ELSE) {
        if (node->first->value_type != AST_TYPE_BOOL) {
            return ERR(AST_RES_ERR_INVALID_LEFT_TYPE);
        }
    } else if (node->node_type == AST_DO_WHILE) {
        if (node->right->value_type != AST_TYPE_BOOL) {
            return ERR(AST_RES_ERR_INVALID_RIGHT_TYPE);
        }
    } else {
        assert(false);
    }

    return OK(AST_TYPE_VOID);
}

#define boolToPTR(z) ((z) ? (void*)0x1 : (void*)0x0)

ASTResult parenthesesLvalChecker(ASTNode* node) {
    return OK(boolToPTR(node->child->allowed_lval));
}

ASTResult ternaryCondLvalChecker(ASTNode* node) {
    return OK(boolToPTR(node->second->allowed_lval && node->third->allowed_lval));
}

ASTResult assignmentLvalChecker(ASTNode* node) {
    if (!node->left->allowed_lval) {
        return ERR(AST_RES_ERR_INVALID_LVAL);
    }
    return OK(boolToPTR(false));
}

ASTResult unaryAssignmentLvalChecker(ASTNode* node) {
    if (!node->child->allowed_lval) {
        return ERR(AST_RES_ERR_INVALID_LVAL);
    }
    return OK(boolToPTR(false));
}

// Lookup Table
ASTNodeInfo ASTNodeTable[] = {
    [AST_INT]            = {"AST_INT",            ZEROARY_OP, false, NULL, NULL},
    [AST_BOOL]           = {"AST_BOOL",           ZEROARY_OP, false, NULL, NULL},
    [AST_ADD]            = {"AST_ADD",            BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_SUB]            = {"AST_SUB",            BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_MUL]            = {"AST_MUL",            BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_DIV]            = {"AST_DIV",            BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_MOD]            = {"AST_MOD",            BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_USUB]           = {"AST_USUB",           UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_UADD]           = {"AST_UADD",           UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_BITWISE_OR]     = {"AST_BITWISE_OR",     BINARY_OP,  false, &binaryBitwiseExpressionTypeHandler, NULL},
    [AST_BITWISE_AND]    = {"AST_BITWISE_AND",    BINARY_OP,  false, &binaryBitwiseExpressionTypeHandler, NULL},
    [AST_BITWISE_XOR]    = {"AST_BITWISE_XOR",    BINARY_OP,  false, &binaryBitwiseExpressionTypeHandler, NULL},
    [AST_BITWISE_NOT]    = {"AST_BITWISE_NOT",    UNARY_OP,   false, &unaryBitwiseExpressionTypeHandler, NULL},
    [AST_L_SHIFT]        = {"AST_L_SHIFT",        BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_R_SHIFT]        = {"AST_R_SHIFT",        BINARY_OP,  false, &binaryExpressionTypeHandler, NULL},
    [AST_ABS]            = {"AST_ABS",            UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_SET_POSITIVE]   = {"AST_SET_POSITIVE",   UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_SET_NEGATIVE]   = {"AST_SET_NEGATIVE",   UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_LOGICAL_NOT]    = {"AST_LOGICAL_NOT",    UNARY_OP,   false, &unaryBooleanExpressionTypeHandler, NULL},
    [AST_LOGICAL_AND]    = {"AST_LOGICAL_AND",    BINARY_OP,  false, &binaryBooleanExpressionTypeHandler, NULL},
    [AST_LOGICAL_OR]     = {"AST_LOGICAL_OR",     BINARY_OP,  false, &binaryBooleanExpressionTypeHandler, NULL},
    [AST_ID]             = {"AST_ID",             ZEROARY_OP, false, NULL, NULL},
    [AST_ID_DECLARATION] = {"AST_ID_DECLARATION", UNARY_OP,   true,  &genericStatementTypeHandler, NULL},
    [AST_ID_DECL_ASSIGN] = {"AST_ID_DECL_ASSIGN", BINARY_OP,  true,  &declAssignmentTypeHandler, NULL},
    [AST_ID_ASSIGNMENT]  = {"AST_ID_ASSIGNMENT",  BINARY_OP,  false, &assignmentTypeHandler, &assignmentLvalChecker},
    [AST_STATEMENT_SEQ]  = {"AST_STATEMENT_SEQ",  BINARY_OP,  true,  &genericStatementTypeHandler, NULL},
    [AST_PRINT]          = {"AST_PRINT",          UNARY_OP,   true,  &genericStatementTypeHandler, NULL},
    [AST_PRINT_VAR]      = {"AST_PRINT_VAR",      UNARY_OP,   true,  &genericStatementTypeHandler, NULL},
    [AST_NO_OP]          = {"AST_NO_OP",          ZEROARY_OP, true,  NULL, NULL},
    [AST_TYPE]           = {"AST_TYPE",           ZEROARY_OP, false, NULL, NULL},
    [AST_TYPE_OF]        = {"AST_TYPE_OF",        UNARY_OP,   false, &typeTypeHandler, NULL},
    [AST_PARENTHESES]    = {"AST_PARENTHESES",    UNARY_OP,   false, &anyUnaryExpressionTypeHandler, &parenthesesLvalChecker},
    [AST_CMP_EQ]         = {"AST_CMP_EQ",         BINARY_OP,  false, &binaryCmpExpressionTypeHandler, NULL},
    [AST_CMP_NEQ]        = {"AST_CMP_NEQ",        BINARY_OP,  false, &binaryCmpExpressionTypeHandler, NULL},
    [AST_CMP_LT]         = {"AST_CMP_LT",         BINARY_OP,  false, &binaryCmpExpressionTypeHandler, NULL},
    [AST_CMP_LTE]        = {"AST_CMP_LTE",        BINARY_OP,  false, &binaryCmpExpressionTypeHandler, NULL},
    [AST_CMP_GT]         = {"AST_CMP_GT",         BINARY_OP,  false, &binaryCmpExpressionTypeHandler, NULL},
    [AST_CMP_GTE]        = {"AST_CMP_GTE",        BINARY_OP,  false, &binaryCmpExpressionTypeHandler, NULL},
    [AST_SCOPE]          = {"AST_SCOPE",          UNARY_OP,   true,  &genericStatementTypeHandler, NULL},
    [AST_TERNARY_COND]   = {"AST_TERNARY_COND",   TERNARY_OP, false, &ternaryCondTypeHandler, &ternaryCondLvalChecker},
    [AST_IF]             = {"AST_IF",             BINARY_OP,  true,  &ifTypeHandler, NULL},
    [AST_IF_ELSE]        = {"AST_IF_ELSE",        TERNARY_OP, true,  &ifTypeHandler, NULL},
    [AST_INC]            = {"AST_INC",            UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_DEC]            = {"AST_DEC",            UNARY_OP,   false, &unaryExpressionTypeHandler, NULL},
    [AST_LOGICAL_TOGGLE] = {"AST_LOGICAL_TOGGLE", UNARY_OP,   false, &unaryBooleanExpressionTypeHandler, NULL},
    [AST_BITWISE_TOGGLE] = {"AST_BITWISE_TOGGLE", UNARY_OP,   false, &unaryBitwiseExpressionTypeHandler, NULL},
    [AST_COMPD_ASSIGN]   = {"AST_COMPD_ASSIGN",   UNARY_OP,   false, &anyUnaryExpressionTypeHandler, NULL},
    [AST_WHILE]          = {"AST_WHILE",          BINARY_OP,  true,  &ifTypeHandler, NULL},
    [AST_DO_WHILE]       = {"AST_DO_WHILE",       BINARY_OP,  true,  &ifTypeHandler, NULL},
    [AST_FOR]            = {"AST_FOR",            UNARY_OP,   true,  &genericStatementTypeHandler, NULL},
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
    node->allowed_lval = false;
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

    if (ASTNodeTable[node_type].lval_checker != NULL) {
        res = ASTNodeTable[node_type].lval_checker(node);
        if (isERR(res)) {
            return ERR_VAL(res.result_type, node);
        }
        node->allowed_lval = (bool) res.result_value;
    }

    return OK(node);
}

ASTResult newASTUnaryOP(const ASTNodeType node_type, const ASTNode* child) {
    assert(child != NULL);

    ASTNode* node = newASTNode(node_type, child->size + 1);
    node->child = child;
    node->is_prefix = false;

    ASTResult res = ASTNodeTable[node_type].type_handler(node);
    if(isERR(res)) {
        return ERR_VAL(res.result_type, node);
    }
    node->value_type = (ASTType) res.result_value;

    if (ASTNodeTable[node_type].lval_checker != NULL) {
        res = ASTNodeTable[node_type].lval_checker(node);
        if (isERR(res)) {
            return ERR_VAL(res.result_type, node);
        }
        node->allowed_lval = (bool) res.result_value;
    }

    return OK(node);
}

ASTResult newASTTernaryOP(ASTNodeType node_type, ASTNode* first, ASTNode* second, ASTNode* third) {
    assert(first != NULL && second != NULL && third != NULL);

    ASTNode* node = newASTNode(node_type, first->size + second->size + third->size + 1);
    node->first = first;
    node->second = second;
    node->third = third;

    ASTResult res = ASTNodeTable[node_type].type_handler(node);
    if(isERR(res)) {
        return ERR_VAL(res.result_type, node);
    }
    node->value_type = (ASTType) res.result_value;

    if (ASTNodeTable[node_type].lval_checker != NULL) {
        res = ASTNodeTable[node_type].lval_checker(node);
        if (isERR(res)) {
            return ERR_VAL(res.result_type, node);
        }
        node->allowed_lval = (bool) res.result_value;
    }

    return OK(node);
}

ASTNode* newASTID(Symbol* id) {
    assert(id != NULL);

    ASTNode* node = newASTNode(AST_ID, 1);
    node->value_type = getVarType(id);
    node->id = id;
    node->allowed_lval = true;
    return node;
}

ASTResult newASTIDReference(const char* id, const SymbolTable* st) {
    assert(id != NULL);
    assert(st != NULL);

    ASTResult res = getVarReference(st, id);
    if (isERR(res)) {
        return res;
    }
    Symbol* var = res.result_value;
    return OK(newASTID(var));
}

ASTResult newASTIDDeclaration(ASTType type, const char* id, const ASTNode* value, bool redef, SymbolTable* st) {
    assert(id != NULL);
    assert(st != NULL);

    if (type == AST_TYPE_VOID) {
        return ERR_VAL(AST_RES_ERR_INVALID_TYPE, ASTTypeToStr(type));
    }

    bool is_init = value != NULL;

    ASTResult res = defineVar(st, type, id, redef);
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

ASTNode* newASTNoOp() {
    ASTNode* node = newASTNode(AST_NO_OP, 1);
    node->value_type = AST_TYPE_VOID;
    return node;
}

ASTResult newASTUnaryCompoundAssign(ASTNodeType node_type, const ASTNode* lval, bool is_prefix) {
    assert(lval != NULL);

    if (!lval->allowed_lval) {
        return ERR_VAL(AST_RES_ERR_INVALID_LVAL, lval);
    }

    ASTResult res;
    switch (node_type) {
        case AST_INC:
            res = newASTAdd(lval, newASTInt(1));
            break;
        case AST_DEC:
            res = newASTAdd(lval, newASTInt(-1));
            break;
        case AST_LOGICAL_TOGGLE:
            res = newASTLogicalNot(lval);
            break;
        case AST_BITWISE_TOGGLE:
            res = newASTBitwiseNot(lval);
            break;
        default:
            assert(false);
            break;
    }

    if (isERR(res)) {
        assert(res.result_type != AST_RES_ERR_INVALID_RIGHT_TYPE);
        if (res.result_type == AST_RES_ERR_INVALID_LEFT_TYPE) {
            res.result_type = AST_RES_ERR_INVALID_CHILD_TYPE;
        }
        return res;
    }

    res = newASTAssignment(copyAST(lval), res.result_value);
    if (isERR(res)) {
        return res;
    }

    res = newASTUnaryOP(node_type, res.result_value);
    if (isERR(res)) {
        return res;
    }
    ASTNode* node = res.result_value;
    node->is_prefix = is_prefix;

    return OK(node);
}

ASTResult newASTCompoundAssignment(ASTNodeType node_type, const ASTNode* lval, ASTNode* value) {
    assert(lval != NULL);
    assert(value != NULL);

    ASTResult res;
    switch (node_type) {
        case AST_ADD:
            res = newASTAdd(lval, value);
            break;
        case AST_SUB:
            res = newASTSub(lval, value);
            break;
        case AST_MUL:
            res = newASTMul(lval, value);
            break;
        case AST_DIV:
            res = newASTDiv(lval, value);
            break;
        case AST_MOD:
            res = newASTMod(lval, value);
            break;
        case AST_BITWISE_AND:
            res = newASTBitwiseAnd(lval, value);
            break;
        case AST_BITWISE_OR:
            res = newASTBitwiseOr(lval, value);
            break;
        case AST_BITWISE_XOR:
            res = newASTBitwiseXor(lval, value);
            break;
        case AST_L_SHIFT:
            res = newASTLeftShift(lval, value);
            break;
        case AST_R_SHIFT:
            res = newASTRightShift(lval, value);
            break;
        case AST_LOGICAL_AND:
            res = newASTLogicalAnd(lval, value);
            break;
        case AST_LOGICAL_OR:
            res = newASTLogicalOr(lval, value);
            break;
        default:
            assert(false);
    }

    if (isERR(res)) {
        return res;
    }

    res = newASTAssignment(copyAST(lval), res.result_value);
    if (isERR(res)) {
        return res;
    }

    return newASTUnaryOP(AST_COMPD_ASSIGN, res.result_value);
}

ASTResult newASTFor(const ASTNode* init, const ASTNode* cond, const ASTNode* update, const ASTNode* body) {
    assert(init != NULL);
    assert(cond != NULL);
    assert(update != NULL);
    assert(body != NULL);
    assert(body->node_type == AST_SCOPE || body->node_type == AST_NO_OP);

    const ASTNode* s = newASTScope(newASTStatementList(body, update));

    if (cond->node_type == AST_NO_OP) {
        deleteASTNode((ASTNode**)&cond);
        cond = newASTBool(true);
    }

    ASTResult res = newASTWhile(cond, s);
    if (isERR(res)) {
        return res;
    }
    const ASTNode* loop = res.result_value;

    return newASTUnaryOP(AST_FOR, newASTStatementList(init, loop));
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
        case TERNARY_OP:
            deleteASTNode(&((*node)->first));
            deleteASTNode(&((*node)->second));
            deleteASTNode(&((*node)->third));
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
                case AST_NO_OP:
                    return ast2->node_type == AST_NO_OP;
                default:
                    assert(false);
            }
        } case UNARY_OP:
            return equalAST(ast1->child, ast2->child);
        case BINARY_OP:
            return equalAST(ast1->left, ast2->left) && equalAST(ast1->right, ast2->right);
        case TERNARY_OP:
            return equalAST(ast1->first, ast2->first) && equalAST(ast1->second, ast2->second) && equalAST(ast1->third, ast2->third);
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

bool requireParentheses(const ASTNode* ast) {
    assert(ast != NULL);
    return isCmpExp(ast) || (ast->node_type == AST_TERNARY_COND && ast->allowed_lval);
}

ASTNode* copyAST(const ASTNode* src_ast) {
    assert(src_ast != NULL);

    switch(getNodeOpType(src_ast->node_type)) {
        case ZEROARY_OP: {
            switch (src_ast->node_type) {
                case AST_INT:
                    return newASTInt(src_ast->n);
                case AST_BOOL:
                    return newASTBool(src_ast->z);
                case AST_TYPE:
                    return newASTType(src_ast->t);
                case AST_ID:
                    return newASTID(src_ast->id);
                case AST_NO_OP:
                    return newASTNoOp();
                default:
                    assert(false);
            }
        } case UNARY_OP: {
            ASTResult res = newASTUnaryOP(src_ast->node_type, copyAST(src_ast->child));
            assert(isOK(res));
            return res.result_value;
        }
        case BINARY_OP: {
            ASTResult res = newASTBinaryOP(src_ast->node_type, copyAST(src_ast->left), copyAST(src_ast->right));
            assert(isOK(res));
            return res.result_value;
        }
        case TERNARY_OP: {
            ASTResult res = newASTTernaryOP(src_ast->node_type, copyAST(src_ast->first), copyAST(src_ast->second), copyAST(src_ast->third));
            assert(isOK(res));
            return res.result_value;
        }
        default:
            assert(false);
    }
}