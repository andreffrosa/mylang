#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "ast/ast.h"

#include "out.h"

static inline int getPrecedence(ASTNodeType node_type) {
    switch (node_type) {
        case AST_NUMBER:
            return 11;
        case AST_BITWISE_NOT:
        case AST_USUB:
        case AST_UADD:
        case AST_ABS:
        case AST_SET_POSITIVE:
        case AST_SET_NEGATIVE:
            return 7;
        case AST_MUL:
        case AST_DIV:
        case AST_MOD:
            return 6;
        case AST_ADD:
        case AST_SUB:
            return 5;
        case AST_L_SHIFT:
        case AST_R_SHIFT:
            return 4;
        case AST_BITWISE_AND:
            return 3;
        case AST_BITWISE_XOR:
            return 2;
        case AST_BITWISE_OR:
            return 1;
        default:
            assert(false);
    }
}

static inline bool needParentheses(ASTNodeType current_type, ASTNodeType child_type, bool is_left_child) {
    if(current_type == child_type) {
        return false;
    }

    int current_precedence = getPrecedence(current_type);
    int child_precedence = getPrecedence(child_type);
    if(child_precedence < current_precedence) {
        return true;
    }

    bool are_not_commutative = (current_type == AST_SUB || current_type == AST_DIV || current_type == AST_MOD);
    return child_precedence == current_precedence && !is_left_child && are_not_commutative;
}


static inline void compileBinaryOP(ASTNode* node, const char* symbol, IOStream* stream) {
    bool need_parentheses = needParentheses(node->type, node->left->type, true);
    if(need_parentheses) {IOStreamWritef(stream, "(");}
    outCompileExpression(node->left, stream);
    if(need_parentheses) {IOStreamWritef(stream, ")");}

    IOStreamWritef(stream, symbol);

    need_parentheses = needParentheses(node->type, node->right->type, false);
    if(need_parentheses) {IOStreamWritef(stream, "(");}
    outCompileExpression(node->right, stream);
    if(need_parentheses) {IOStreamWritef(stream, ")");}
}

int outCompileExpression(ASTNode* node, IOStream* stream) {
    assert(node != NULL);

    switch (node->type) {
        case AST_NUMBER:
            IOStreamWritef(stream, "%d", node->n);
            break;
        case AST_ADD:
            compileBinaryOP(node, " + ", stream);
            break;
        case AST_SUB:
            compileBinaryOP(node, " - ", stream);
            break;
        case AST_MUL:
            compileBinaryOP(node, "*", stream);
            break;
        case AST_DIV:
            compileBinaryOP(node, "/", stream);
            break;
        case AST_MOD:
            compileBinaryOP(node, "%%", stream);
            break;
        case AST_USUB:
            IOStreamWritef(stream, "-");
            outCompileExpression(node->child, stream);
            break;
        case AST_UADD:
            IOStreamWritef(stream, "+");
            outCompileExpression(node->child, stream);
            break;
        case AST_ABS: {
            IOStreamWritef(stream, "abs(");
            outCompileExpression(node->child, stream);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_POSITIVE: {
            IOStreamWritef(stream, "abs(");
            outCompileExpression(node->child, stream);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_NEGATIVE: {
            IOStreamWritef(stream, "-abs(");
            outCompileExpression(node->child, stream);
            IOStreamWritef(stream, ")");
            break;
        } case AST_BITWISE_AND:
            compileBinaryOP(node, "&", stream);
            break;
        case AST_BITWISE_OR:
            compileBinaryOP(node, "|", stream);
            break;
        case AST_BITWISE_XOR:
            compileBinaryOP(node, "^", stream);
            break;
        case AST_BITWISE_NOT:
            IOStreamWritef(stream, "~");
            outCompileExpression(node->child, stream);
            break;
        case AST_L_SHIFT:
            compileBinaryOP(node, " << ", stream);
            break;
        case AST_R_SHIFT:
            compileBinaryOP(node, " >> ", stream);
            break;
        default:
            assert(false);
    }
}

