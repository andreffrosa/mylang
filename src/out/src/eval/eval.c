#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "out.h"

#include "frame.h"

Frame* executeAST(const ASTNode* ast, const SymbolTable* st) {
    assert(ast != NULL && st != NULL);
    unsigned int frame_size = getMaxOffset(st) + 1;
    Frame* frame = newFrame(frame_size);
    executeASTStatements(ast, st, frame);
    return frame;
}

int evalID(const SymbolTable* st, const Symbol* var, const Frame* frame) {
    assert(st != NULL  && var != NULL && frame != NULL);

    assert(isVarInitialized(var));
    unsigned int index = getVarOffset(var);
    return getFrameValue(frame, index);
}

int evalAssignment(const ASTNode* ast, const SymbolTable* st, Frame* frame) {
    assert(ast->left->node_type == AST_ID);
    assert(ast->left->id != NULL);

    Symbol* var = ast->left->id;
    int value = evalASTExpression(ast->right, st, frame);
    unsigned int index = getVarOffset(var);
    setFrameValue(frame, index, value);

    return value;
}

static inline bool compare(const ASTNodeType node_type, const int l, const int r) {
    switch (node_type) {
        case AST_CMP_EQ: return l == r;
        case AST_CMP_NEQ: return l != r;
        case AST_CMP_LT: return l < r;
        case AST_CMP_LTE: return l <= r;
        case AST_CMP_GT: return l > r;
        case AST_CMP_GTE: return l >= r;
        default:
            assert(false);
    }
}

bool evalCmpExpression(const ASTNode* ast, const SymbolTable* st, Frame* frame, int* carry) {
    if(isCmpExp(ast->left)) { // is chained
        if(!evalCmpExpression(ast->left, st, frame, carry)) {
            return false;
        }
        int l = *carry;
        int r = evalASTExpression(ast->right, st, frame);
        *carry = r;
        return compare(ast->node_type, l, r);
    }
    int l = evalASTExpression(ast->left, st, frame);
    int r = evalASTExpression(ast->right, st, frame);
    *carry = r;
    return compare(ast->node_type, l, r);
}

void executeASTStatements(const ASTNode* ast, const SymbolTable* st, Frame* frame) {
    assert(ast != NULL && st != NULL);

    switch (ast->node_type) {
        case AST_ID_DECLARATION: {
            assert(ast->child->node_type == AST_ID);
            assert(ast->child->id != NULL);
            break;
        } case AST_ID_DECL_ASSIGN: {
            assert(ast->left->node_type == AST_ID);
            assert(ast->left->id != NULL);

            Symbol* var = ast->left->id;
            int value = evalASTExpression(ast->right, st, frame);
            unsigned int index = getVarOffset(var);
            setFrameValue(frame, index, value);
            break;
        } case AST_ID_ASSIGNMENT: {
            evalAssignment(ast, st, frame);
            break;
        } case AST_STATEMENT_SEQ: {
            executeASTStatements(ast->left, st, frame);
            executeASTStatements(ast->right, st, frame);
            break;
        } case AST_PRINT: {
            const int value = evalASTExpression(ast->child, st, frame);
            char buffer[TYPE_VALUE_BUFFER_SIZE];
            ASTTypeValueToStr(ast->child->value_type, value, buffer);
            printf("%s\n", buffer);
            break;
        } case AST_PRINT_VAR: {
            assert(ast->child->node_type == AST_ID);
            const int value = evalASTExpression(ast->child, st, frame);
            IOStream* s = openIOStreamFromStdout();
            printVar(ast->child->id, &value, s);
            IOStreamWritef(s, "\n");
            IOStreamClose(&s);
            break;
        } case AST_SCOPE: {
            executeASTStatements(ast->child, st, frame);
            break;
        } case AST_IF: {
            if(evalASTExpression(ast->left, st, frame)) {
                executeASTStatements(ast->right, st, frame);
            }
            break;
        } case AST_IF_ELSE: {
            if(evalASTExpression(ast->first, st, frame)) {
                executeASTStatements(ast->second, st, frame);
            } else {
                executeASTStatements(ast->third, st, frame);
            }
            break;
        } case AST_NO_OP: break;
          default: {
            if(isExp(ast)) {
                evalASTExpression(ast, st, frame);
            } else {
                assert(false);
            }
        }
    }
}

int evalASTExpression(const ASTNode* node, const SymbolTable* st, Frame* frame) {
    assert(node != NULL);
    switch (node->node_type) {
        case AST_INT:
            return node->n;
        case AST_BOOL:
            return node->z;
        case AST_TYPE:
            return node->t;
        case AST_ADD:
            return evalASTExpression(node->left, st, frame) + evalASTExpression(node->right, st, frame);
        case AST_SUB:
            return evalASTExpression(node->left, st, frame) - evalASTExpression(node->right, st, frame);
        case AST_MUL:
            return evalASTExpression(node->left, st, frame) * evalASTExpression(node->right, st, frame);
        case AST_DIV:
            return evalASTExpression(node->left, st, frame) / evalASTExpression(node->right, st, frame);
        case AST_MOD:
            return evalASTExpression(node->left, st, frame) % evalASTExpression(node->right, st, frame);
        case AST_USUB:
            return - evalASTExpression(node->child, st, frame);
        case AST_UADD:
            return + evalASTExpression(node->child, st, frame);
        case AST_BITWISE_OR:
            return evalASTExpression(node->left, st, frame) | evalASTExpression(node->right, st, frame);
        case AST_BITWISE_AND:
            return evalASTExpression(node->left, st, frame) & evalASTExpression(node->right, st, frame);
        case AST_BITWISE_XOR:
            return evalASTExpression(node->left, st, frame) ^ evalASTExpression(node->right, st, frame);
        case AST_BITWISE_NOT: {
            int result = evalASTExpression(node->child, st, frame);
            return node->child->value_type == AST_TYPE_BOOL ? !result : ~ result;
        }
        case AST_L_SHIFT:
            return evalASTExpression(node->left, st, frame) << evalASTExpression(node->right, st, frame);
        case AST_R_SHIFT:
            return evalASTExpression(node->left, st, frame) >> evalASTExpression(node->right, st, frame);
        case AST_ABS: {
            int v = evalASTExpression(node->child, st, frame);
            return v >= 0 ? v : - v;
        } case AST_SET_POSITIVE: {
            int v = evalASTExpression(node->child, st, frame);
            return (v < 0)*(~(v)+1) + (1 - (v < 0))*v;
        } case AST_SET_NEGATIVE: {
            int v = evalASTExpression(node->child, st, frame);
            return (1 - (v < 0))*(~(v)+1) + (v < 0)*v;
        } case AST_ID: {
            Symbol* var = node->id;
            assert(isVarInitialized(var));
            return evalID(st, var, frame);
        } case AST_LOGICAL_NOT: {
            return !evalASTExpression(node->child, st, frame);
        } case AST_LOGICAL_AND: {
            bool l = evalASTExpression(node->left, st, frame);
            if(l) {
                return evalASTExpression(node->right, st, frame);
            }
            return false;
        } case AST_LOGICAL_OR: {
            bool l = evalASTExpression(node->left, st, frame);
            if(!l) {
                return evalASTExpression(node->right, st, frame);
            }
            return true;
        } case AST_ID_ASSIGNMENT: {
            return evalAssignment(node, st, frame);
        } case AST_TYPE_OF: {
            // TODO: [optimization] check if it needs eval (only needs if has side-effects)
            evalASTExpression(node->child, st, frame);
            return node->child->value_type;
        } case AST_PARENTHESES: {
            return evalASTExpression(node->child, st, frame);
        } case AST_CMP_EQ:
          case AST_CMP_NEQ:
          case AST_CMP_LT:
          case AST_CMP_LTE:
          case AST_CMP_GT:
          case AST_CMP_GTE: {
            int carry = 0;
            return evalCmpExpression(node, st, frame, &carry);
        } case AST_TERNARY_COND: {
            return evalASTExpression(node->first, st, frame) ? evalASTExpression(node->second, st, frame) : evalASTExpression(node->third, st, frame);
        } default:
            assert(false);
    }
}