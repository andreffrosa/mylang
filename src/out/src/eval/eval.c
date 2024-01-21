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

void executeASTStatements(const ASTNode* ast, const SymbolTable* st, Frame* frame) {
    assert(ast != NULL && st != NULL);

    switch (ast->type) {
        case AST_ID_DECLARATION: {
            assert(ast->child->type == AST_ID);
            assert(ast->child->id != NULL);
            break;
        } case AST_ID_DECL_ASSIGN: {
            assert(ast->left->type == AST_ID);
            assert(ast->left->id != NULL);

            Symbol* var = ast->left->id;
            int value = evalASTExpression(ast->right, st, frame);
            unsigned int index = getVarIndex(st, var);
            setFrameValue(frame, index, value);
            break;
        } case AST_ID_ASSIGNMENT: {
            assert(ast->left->type == AST_ID);
            assert(ast->left->id != NULL);

            Symbol* var = ast->left->id;
            int value = evalASTExpression(ast->right, st, frame);
            unsigned int index = getVarIndex(st, var);
            setFrameValue(frame, index, value);
            break;
        } case AST_STATEMENT_SEQ: {
            executeASTStatements(ast->left, st, frame);
            executeASTStatements(ast->right, st, frame);
            break;
        } default: {
            if(isExp(ast)) {
                evalASTExpression(ast, st, frame);
            } else {
                assert(false);
            }
        }
    }
}

int evalASTExpression(const ASTNode* node, const SymbolTable* st, const Frame* frame) {
    assert(node != NULL);
    switch (node->type) {
        case AST_NUMBER:
            return node->n;
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
        case AST_BITWISE_NOT:
            return ~ evalASTExpression(node->child, st, frame);
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
        } default:
            assert(false);
    }
}