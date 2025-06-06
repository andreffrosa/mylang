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

    //assert(isVarInitialized(var));
    unsigned int index = getVarOffset(var);
    return getFrameValue(frame, index);
}

static Symbol* evalLVal(const ASTNode* lval, const SymbolTable* st, Frame* frame) {
    assert(lval != NULL);

    if (lval->node_type == AST_ID) {
        return lval->id;
    } else if (lval->node_type == AST_TERNARY_COND) {
        bool cond = evalASTExpression(lval->first, st, frame);
        return evalLVal(cond ? lval->second : lval->third, st, frame);
    } else if (lval->node_type == AST_PARENTHESES) {
        return evalLVal(lval->child, st, frame);
    } else {
        printf("evalLVal not defined for %s\n", nodeTypeToStr(lval->node_type));
        assert(false);
    }
}

int evalAssignment(const ASTNode* ast, const SymbolTable* st, Frame* frame) {
    assert(ast != NULL);
    assert(st != NULL);
    assert(frame != NULL);

    Symbol* var = evalLVal(ast->left, st, frame);

    int value = evalASTExpression(ast->right, st, frame);
    unsigned int index = getVarOffset(var);
    setFrameValue(frame, index, value);

    return value;
}

static inline bool compare(const ASTNodeType node_type, const int l, const int r) {
    switch (node_type) {
        case AST_CMP_EQ:  return l == r;
        case AST_CMP_NEQ: return l != r;
        case AST_CMP_LT:  return l <  r;
        case AST_CMP_LTE: return l <= r;
        case AST_CMP_GT:  return l >  r;
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



EvalStatus executeASTStatements(const ASTNode* ast, const SymbolTable* st, Frame* frame) {
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
        } case AST_STATEMENT_SEQ: {
            EvalStatus s = executeASTStatements(ast->left, st, frame);
            if (s.status) {
                return s;
            }
            s = executeASTStatements(ast->right, st, frame);
            if (s.status) {
                return s;
            }
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
            EvalStatus s = executeASTStatements(ast->child, st, frame);
            if (s.status) {
                return s;
            }
            break;
        } case AST_IF: {
            if(evalASTExpression(ast->left, st, frame)) {
                EvalStatus s = executeASTStatements(ast->right, st, frame);
                if (s.status) {
                    return s;
                }
            }
            break;
        } case AST_IF_ELSE: {
            if(evalASTExpression(ast->first, st, frame)) {
                EvalStatus s = executeASTStatements(ast->second, st, frame);
                if (s.status) {
                    return s;
                }
            } else {
                EvalStatus s = executeASTStatements(ast->third, st, frame);
                if (s.status) {
                    return s;
                }
            }
            break;
        }
        case AST_NO_OP: break;
        case AST_WHILE: {
            assert(ast->right->node_type == AST_SCOPE);
            EvalStatus s;
            while (evalASTExpression(ast->left, st, frame)) {
                s = executeASTStatements(ast->right, st, frame);
                if (s.status) {
                    if (s.node_type == AST_BREAK) {
                        break;
                    } else if (s.node_type == AST_CONTINUE) {
                    } else {
                        assert(false);
                    }
                }
            }
            break;
        }
        case AST_DO_WHILE: {
            assert(ast->left->node_type == AST_SCOPE);
            EvalStatus s;
            do {
                s = executeASTStatements(ast->left, st, frame);
                if (s.status) {
                    if (s.node_type == AST_BREAK) {
                        break;
                    } else if (s.node_type == AST_CONTINUE) {
                    } else {
                        assert(false);
                    }
                }
            } while (evalASTExpression(ast->right, st, frame));
            break;
        }
        case AST_FOR: {
            //executeASTStatements(ast->child, st, frame);
            const ASTNode* init = ast->child->left;
            const ASTNode* cond = ast->child->right->left;
            const ASTNode* scope = ast->child->right->right;
            assert(scope->node_type == AST_SCOPE);
            const ASTNode* update = scope->child->right;
            const ASTNode* body = scope->child->left;

            EvalStatus s;
            executeASTStatements(init, st, frame);
            while(evalASTExpression(cond, st, frame)) {
                s = executeASTStatements(body, st, frame);
                if (s.status) {
                    if (s.node_type == AST_BREAK) {
                        break;
                    } else if (s.node_type == AST_CONTINUE) {
                    } else {
                        assert(false);
                    }
                }
                executeASTStatements(update, st, frame);
            }
            break;
        }
        case AST_BREAK: {
            return (EvalStatus) {
                .status = true,
                .node_type = AST_BREAK,
            };
        }
        case AST_CONTINUE: {
            return (EvalStatus) {
                .status = true,
                .node_type = AST_CONTINUE,
            };
        }
        default: {
            if(isExp(ast)) {
                evalASTExpression(ast, st, frame);
            } else {
                assert(false);
            }
        }
    }

    return (EvalStatus) {
        .status = false,
        .node_type = AST_NODE_TYPES_COUNT
    };
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
        } case AST_INC: {
            int n = evalASTExpression(node->child, st, frame);
            return node->is_prefix ? n : n - 1;
        } case AST_DEC: {
            int n = evalASTExpression(node->child, st, frame);
            return node->is_prefix ? n : n + 1;
        } case AST_LOGICAL_TOGGLE: {
            bool z = evalASTExpression(node->child, st, frame);
            return node->is_prefix ? z : !z;
        } case AST_BITWISE_TOGGLE: {
            int n = evalASTExpression(node->child, st, frame);
            return node->is_prefix ? n : (node->child->value_type == AST_TYPE_BOOL ? !n : ~ n);
        } case AST_COMPD_ASSIGN: {
            return evalASTExpression(node->child, st, frame);
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