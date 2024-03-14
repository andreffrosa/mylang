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
    Frame* frame = newFrame(getSymbolTableSize(st));
    executeASTStatements(ast, st, frame);
    return frame;
}

int evalID(const SymbolTable* st, const Symbol* var, const Frame* frame) {
    assert(st != NULL  && var != NULL && frame != NULL);

    assert(isVarInitialized(var));
    unsigned int index = getVarIndex(st, var);
    return getFrameValue(frame, index);
}

int evalAssignment(const ASTNode* ast, const SymbolTable* st, Frame* frame) {
    assert(ast->left->node_type == AST_ID);
    assert(ast->left->id != NULL);

    Symbol* var = ast->left->id;
    int value = evalASTExpression(ast->right, st, frame);
    unsigned int index = getVarIndex(st, var);
    setFrameValue(frame, index, value);

    return value;
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
            unsigned int index = getVarIndex(st, var);
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
        } default:
            assert(false);
    }
}