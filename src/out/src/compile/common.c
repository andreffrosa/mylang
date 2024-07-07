#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ast/ast.h"

#include "out.h"

static inline int getPrecedence(ASTNodeType node_type) {
    switch (node_type) {
        case AST_INT:
        case AST_BOOL:
        case AST_ID:
            return 12;
        case AST_LOGICAL_NOT:
        case AST_BITWISE_NOT:
        case AST_USUB:
        case AST_UADD:
        case AST_ABS:
        case AST_SET_POSITIVE:
        case AST_SET_NEGATIVE:
            return 11;
        case AST_MUL:
        case AST_DIV:
        case AST_MOD:
            return 10;
        case AST_ADD:
        case AST_SUB:
            return 9;
        case AST_L_SHIFT:
        case AST_R_SHIFT:
            return 8;
        case AST_BITWISE_AND:
            return 7;
        case AST_BITWISE_XOR:
            return 6;
        case AST_BITWISE_OR:
            return 5;
        case AST_LOGICAL_AND:
            return 4;
        case AST_LOGICAL_OR:
            return 3;
        default:
            assert(false);
    }
}

static inline bool needParentheses(ASTNodeType current_node_type, ASTNodeType child_node_type, bool is_left_child) {
    if(current_node_type == child_node_type) {
        return false;
    }

    int current_precedence = getPrecedence(current_node_type);
    int child_precedence = getPrecedence(child_node_type);
    if(child_precedence < current_precedence) {
        return true;
    }

    bool are_not_commutative = (current_node_type == AST_SUB || current_node_type == AST_DIV || current_node_type == AST_MOD);
    return child_precedence == current_precedence && !is_left_child && are_not_commutative;
}


static inline void compileBinaryOP(const ASTNode* node, const char* op_symbol, const SymbolTable* st, const OutSerializer* os, const IOStream* stream) {
    bool need_parentheses = needParentheses(node->node_type, node->left->node_type, true);
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->left, st, stream, os);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }

    IOStreamWritef(stream, op_symbol);

    need_parentheses = needParentheses(node->node_type, node->right->node_type, false);
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->right, st, stream, os);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }
}

void compileAssignment(const ASTNode* ast, const SymbolTable* st, const OutSerializer* os, const IOStream* stream) {
    assert(ast->left->node_type == AST_ID);

    Symbol* var = ast->left->id;
    IOStreamWritef(stream, "%s = ", getVarId(var));
    compileASTExpression(ast->right, st, stream, os);
}

void compileIDDeclaration(Symbol* var, const ASTNode* value, const SymbolTable* st, const IOStream* stream, const OutSerializer* os) {
    assert(var != NULL &&  st != NULL);

    os->parseType(stream, getVarType(var), false);
    IOStreamWritef(stream, " %s", getVarId(var));

    if(value != NULL) {
        IOStreamWritef(stream, " = ");
        compileASTExpression(value, st, stream, os);
    }
}

static inline const char* compare(const ASTNodeType node_type) {
    switch (node_type) {
        case AST_CMP_EQ: return "==";
        case AST_CMP_NEQ: return "!=";
        case AST_CMP_LT: return "<";
        case AST_CMP_LTE: return "<=";
        case AST_CMP_GT: return ">";
        case AST_CMP_GTE: return ">=";
        default:
            assert(false);
    }
}

static inline bool needsTempVar(const ASTNodeType node_type) {
    switch (node_type) {
        case AST_INT:
        case AST_BOOL:
        case AST_TYPE:
        case AST_ID:
            return false;
        default:
            return true;
    }
}

void compileCmpExp(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, const bool is_chained) {
    assert(stream != NULL);

    if(isCmpExp(ast->left)) { // left is chained
        compileCmpExp(ast->left, st, stream, os, true);
    } else {
        compileASTExpression(ast->left, st, stream, os);
    }

    IOStreamWritef(stream, " %s ", compare(ast->node_type));

    if(is_chained) {
        if(needsTempVar(ast->right->node_type)) {
            const char* tmp_var_type = ASTTypeToStr(ast->right->value_type);
            IOStreamWritef(stream, "(_tmp_%s = ", tmp_var_type);
            compileASTExpression(ast->right, st, stream, os);
            IOStreamWritef(stream, ") && _tmp_%s", tmp_var_type);
        } else {
            compileASTExpression(ast->right, st, stream, os);
            IOStreamWritef(stream, " && ");
            compileASTExpression(ast->right, st, stream, os);
        }
    } else {
        compileASTExpression(ast->right, st, stream, os);
    }
}

void compileASTExpression(const ASTNode* node, const SymbolTable* st, const IOStream* stream, const OutSerializer* os) {
    assert(node != NULL);

    switch (node->node_type) {
        case AST_INT:
            IOStreamWritef(stream, "%d", node->n);
            break;
        case AST_BOOL:
            IOStreamWritef(stream, "%s", node->z ? "true" : "false");
            break;
        case AST_TYPE:
            os->parseType(stream, node->t, true);
            break;
        case AST_ID:
            IOStreamWritef(stream, "%s", node->id);
            break;
        case AST_ADD:
            compileBinaryOP(node, " + ", st, os, stream);
            break;
        case AST_SUB:
            compileBinaryOP(node, " - ", st, os, stream);
            break;
        case AST_MUL:
            compileBinaryOP(node, "*", st, os, stream);
            break;
        case AST_DIV:
            compileBinaryOP(node, "/", st, os, stream);
            break;
        case AST_MOD:
            compileBinaryOP(node, "%%", st, os, stream);
            break;
        case AST_USUB:
            IOStreamWritef(stream, "-");
            compileASTExpression(node->child, st, stream, os);
            break;
        case AST_UADD:
            IOStreamWritef(stream, "+");
            compileASTExpression(node->child, st, stream, os);
            break;
        case AST_ABS: {
            IOStreamWritef(stream, "abs(");
            compileASTExpression(node->child, st, stream, os);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_POSITIVE: {
            IOStreamWritef(stream, "abs(");
            compileASTExpression(node->child, st, stream, os);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_NEGATIVE: {
            IOStreamWritef(stream, "-abs(");
            compileASTExpression(node->child, st, stream, os);
            IOStreamWritef(stream, ")");
            break;
        } case AST_BITWISE_AND:
            compileBinaryOP(node, "&", st, os, stream);
            break;
        case AST_BITWISE_OR:
            compileBinaryOP(node, "|", st, os, stream);
            break;
        case AST_BITWISE_XOR:
            compileBinaryOP(node, "^", st, os, stream);
            break;
        case AST_BITWISE_NOT:
            if(node->child->value_type == AST_TYPE_BOOL) {
                IOStreamWritef(stream, "!");
            } else {
                IOStreamWritef(stream, "~");
            }
            compileASTExpression(node->child, st, stream, os);
            break;
        case AST_L_SHIFT:
            compileBinaryOP(node, " << ", st, os, stream);
            break;
        case AST_R_SHIFT:
            compileBinaryOP(node, " >> ", st, os, stream);
            break;
        case AST_LOGICAL_NOT:
            IOStreamWritef(stream, "!");
            compileASTExpression(node->child, st, stream, os);
            break;
        case AST_LOGICAL_AND:
            compileBinaryOP(node, " && ", st, os, stream);
            break;
        case AST_LOGICAL_OR:
            compileBinaryOP(node, " || ", st, os, stream);
            break;
        case AST_ID_ASSIGNMENT:
            IOStreamWritef(stream, "(");
            compileAssignment(node, st, os, stream);
            IOStreamWritef(stream, ")");
            break;
        case AST_TYPE_OF: {
            // TODO: [optimization] it is only worth serializing the exp if it has side-effets. otherwise, only the type matters
            // For that, the type system needs to compute whether an expression is pure

            // We can at least check if child is primitive bool/int/type or ID (trivial cases)

            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            compileASTExpression(node->child, st, s, os);
            IOStreamClose(&s);

            os->typeOf(stream, node->child, ptr);
            free(ptr);
            break;
        } case AST_PARENTHESES:
            IOStreamWritef(stream, "(");
            compileASTExpression(node->child, st, stream, os);
            IOStreamWritef(stream, ")");
            break;
        case AST_CMP_EQ:
        case AST_CMP_NEQ:
        case AST_CMP_LT:
        case AST_CMP_LTE:
        case AST_CMP_GT:
        case AST_CMP_GTE: {
            compileCmpExp(node, st, stream, os, false);
            break;
        }
        default:
            assert(false);
    }
}

void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, unsigned int indentation_level) {
    assert(ast != NULL && st != NULL);

    if(ast->node_type == AST_STATEMENT_SEQ) {
        compileASTStatements(ast->left, st, stream, os, indentation_level);
        compileASTStatements(ast->right, st, stream, os, indentation_level);
        return;
    }

    indent(stream, indentation_level);
    switch (ast->node_type) {
        case AST_ID_DECLARATION: {
            assert(ast->child->node_type == AST_ID);
            Symbol* var = ast->child->id;
            compileIDDeclaration(var, NULL, st, stream, os);
            break;
        } case AST_ID_DECL_ASSIGN: {
            assert(ast->left->node_type == AST_ID);
            Symbol* var = ast->left->id;
            compileIDDeclaration(var, ast->right, st, stream, os);
            break;
        } case AST_ID_ASSIGNMENT: {
            compileAssignment(ast, st, os, stream);
            break;
        } case AST_PRINT:
          case AST_PRINT_VAR: {
            assert(os->print != NULL);
            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            compileASTExpression(ast->child, st, s, os);
            IOStreamClose(&s);

            os->print(ptr, ast->child->value_type, ast->node_type == AST_PRINT_VAR, stream);

            free(ptr);
            break;
        } case AST_NO_OP: break;
          default: {
            if(isExp(ast)) {
                compileASTExpression(ast, st, stream, os);
            } else {
                assert(false);
            }
            break;
        }
    }
    IOStreamWritef(stream, ";\n");
}
