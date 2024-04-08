#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ast/ast.h"
#include "out.h"

static inline int getPrecedence(const ASTNode* node) {
    assert(node != NULL);

    switch (node->node_type) {
        case AST_INT:
        case AST_BOOL:
        case AST_ID:
        case AST_PARENTHESES:
            return 14;
        case AST_INC:
        case AST_DEC:
            return node->is_prefix ? 13 : 14;
        case AST_LOGICAL_NOT:
        case AST_BITWISE_NOT:
        case AST_USUB:
        case AST_UADD:
        case AST_ABS:
        case AST_SET_POSITIVE:
        case AST_SET_NEGATIVE:
            return 13;
        case AST_MUL:
        case AST_DIV:
        case AST_MOD:
            return 12;
        case AST_ADD:
        case AST_SUB:
            return 11;
        case AST_L_SHIFT:
        case AST_R_SHIFT:
            return 10;
        case AST_CMP_EQ:
        case AST_CMP_NEQ:
            return 9;
        case AST_CMP_LT:
        case AST_CMP_LTE:
        case AST_CMP_GT:
        case AST_CMP_GTE:
            return 9;
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
        case AST_TERNARY_COND:
            return 2;
        case AST_ID_ASSIGNMENT:
        case AST_COMPD_ASSIGN:
            return 1;
        default:
            printf("Precedence not defined for %s\n", nodeTypeToStr(node->node_type));
            assert(false);
    }
}

static inline bool isComutative(const ASTNode* node) {
    assert(node != NULL);

    switch (node->node_type) {
        case AST_ADD:
        case AST_MUL:
        case AST_LOGICAL_AND:
        case AST_LOGICAL_OR:
            return true;
        default:
            return false;
    }
}

static inline bool isLeftAssociative(const ASTNode* node, const ASTNode* child_node) {
    assert(node != NULL);
    assert(child_node != NULL);
    assert(getPrecedence(node) == getPrecedence(child_node));

    switch (node->node_type) {
        case AST_TERNARY_COND:
        case AST_ID_ASSIGNMENT:
        case AST_COMPD_ASSIGN:
        case AST_UADD:
        case AST_USUB:
        case AST_ABS:
        case AST_SET_POSITIVE:
        case AST_SET_NEGATIVE:
        case AST_BITWISE_NOT:
        case AST_LOGICAL_NOT:
            return false;
        case AST_INC:
        case AST_DEC:
            return !node->is_prefix;
        case AST_PARENTHESES:
            assert(false);
        default:
            return true;
    }
}

static inline bool needParentheses(const ASTNode* current_node, const ASTNode* child_node) {
    assert(current_node != NULL);
    assert(child_node != NULL);

    // Verify if should force parentheses for clarity
    bool force_parentheses = current_node->node_type == AST_TERNARY_COND
                          && child_node->node_type   == AST_TERNARY_COND;
    if (force_parentheses) { return true; }

    int current_precedence = getPrecedence(current_node);
    int child_precedence   = getPrecedence(child_node);
    if (child_precedence < current_precedence) {
        return true;
    } else if (child_precedence > current_precedence) {
        return false;
    }

    if (isComutative(current_node)) {
        return false;
    }

    bool is_left_child = false;
    switch (getNodeOpType(current_node->node_type)) {
        case UNARY_OP:
            is_left_child = false;
            break;
        case BINARY_OP:
            is_left_child = (current_node->left == child_node);
            break;
        case TERNARY_OP:
            is_left_child = (current_node->first == child_node || current_node->second == child_node);
            break;
        default:
            assert(false);
            break;
    }

    bool is_left_assoc = isLeftAssociative(current_node, child_node);
    return is_left_child ? !is_left_assoc : is_left_assoc;
}

static inline void compileChildExpression(const ASTNode* node, const ASTNode* child, const SymbolTable* st, const OutSerializer* os, const IOStream* stream) {
    assert(node != NULL);
    assert(child != NULL);

    bool need_parentheses = needParentheses(node, child);
    if (need_parentheses) { IOStreamWritef(stream, "("); }

    compileASTExpression(child, st, stream, os, false);

    if (need_parentheses) { IOStreamWritef(stream, ")"); }
}

static inline void compileBinaryOP(const ASTNode* node, const char* op_symbol, const SymbolTable* st, const OutSerializer* os, const IOStream* stream) {
    compileChildExpression(node, node->left, st, os, stream);
    IOStreamWritef(stream, op_symbol);
    compileChildExpression(node, node->right, st, os, stream);
}

void printId(const IOStream* stream, const Symbol* var, bool print_redef_level) {
    const char* id = getVarId(var);
    if (print_redef_level) {
        unsigned int redef_level = getVarRedefLevel(var);
        if (redef_level > 0) {
            IOStreamWritef(stream, "%s_%d_", id, redef_level);
        } else {
            IOStreamWritef(stream, "%s", id);
        }
    } else {
        IOStreamWritef(stream, "%s", id);
    }
}

static void writeTmpVar(const ASTType type, const IOStream* stream) {
    const char* tmp_var_type = ASTTypeToStr(type);
    IOStreamWritef(stream, "_tmp_%s", tmp_var_type);
}

void compileAssignment(const ASTNode* node, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool is_stmt) {
    assert(node != NULL);

    const ASTNode* lval = node->left;
    const ASTNode* rval = node->right;

    if (lval->node_type == AST_ID) {
        IOStreamWritef(stream, "%s = ", getVarId(lval->id));
        compileASTExpression(rval, st, stream, os, false);
    } else {
        assert(lval->node_type == AST_PARENTHESES);
        assert(lval->child->node_type == AST_TERNARY_COND);

        if (is_stmt) {
            if (lval->node_type != AST_ID && os->condAssignNeedsTmp()) {
                writeTmpVar(lval->value_type, stream);
                IOStreamWritef(stream, " = ");
            }
            IOStreamWritef(stream, "(");
        }

        ASTResult res = newASTAssignment(copyAST(lval->child->second), copyAST(rval));
        assert(isOK(res));
        ASTNode* l = res.result_value;
        res = newASTAssignment(copyAST(lval->child->third), copyAST(rval));
        assert(isOK(res));
        ASTNode* r = res.result_value;
        res = newASTTernaryCond(copyAST(lval->child->first), l, r);
        assert(isOK(res));
        ASTNode* new_node = res.result_value;
        compileASTExpression(new_node, st, stream, os, false);
        deleteASTNode(&new_node);

        if (is_stmt) {
            IOStreamWritef(stream, ")");
        }
    }
}

void compileCompoundAssignment(const ASTNode* node, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool is_stmt) {
    assert(node != NULL);

    const ASTNodeType op_type = node->child->right->node_type;

    if (!os->hasCompdAssign(op_type)) {
        compileAssignment(node->child, st, os, stream, is_stmt);
        return;
    }

    const ASTNode* lval = node->child->left;
    const ASTNode* rval = node->child->right->right;

    char* op_symbol = NULL;
    switch (op_type) {
        case AST_ADD:           op_symbol =  "+="; break;
        case AST_SUB:           op_symbol =  "-="; break;
        case AST_MUL:           op_symbol =  "*="; break;
        case AST_DIV:           op_symbol =  "/="; break;
        case AST_MOD:           op_symbol =  "%="; break;
        case AST_BITWISE_AND:   op_symbol =  "&="; break;
        case AST_BITWISE_OR:    op_symbol =  "|="; break;
        case AST_BITWISE_XOR:   op_symbol =  "^="; break;
        case AST_L_SHIFT:       op_symbol = "<<="; break;
        case AST_R_SHIFT:       op_symbol = ">>="; break;
        case AST_LOGICAL_AND:   op_symbol = "&&="; break;
        case AST_LOGICAL_OR:    op_symbol = "||="; break;
        default: assert(false);
    }

    if (lval->node_type == AST_ID) {
        IOStreamWritef(stream, "%s %s ", getVarId(lval->id), op_symbol);
        compileASTExpression(rval, st, stream, os, false);
    } else {
        assert(lval->node_type == AST_PARENTHESES);
        assert(lval->child->node_type == AST_TERNARY_COND);

        if (is_stmt) {
            if (lval->node_type != AST_ID && os->condAssignNeedsTmp()) {
                writeTmpVar(lval->value_type, stream);
                IOStreamWritef(stream, " = ");
            }
            IOStreamWritef(stream, "(");
        }

        ASTResult res = newASTCompoundAssignment(op_type, copyAST(lval->child->second), copyAST(rval));
        assert(isOK(res));
        ASTNode* l = res.result_value;
        res = newASTCompoundAssignment(op_type, copyAST(lval->child->third), copyAST(rval));
        assert(isOK(res));
        ASTNode* r = res.result_value;
        res = newASTTernaryCond(copyAST(lval->child->first), l, r);
        assert(isOK(res));
        ASTNode* new_node = res.result_value;
        compileASTExpression(new_node, st, stream, os, false);
        deleteASTNode(&new_node);

        if (is_stmt) {
            IOStreamWritef(stream, ")");
        }
    }
}

void compileUnaryCompoundAssignment(const ASTNode* node, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool is_stmt) {
    assert(node != NULL);

    if (node->child->left->node_type == AST_ID) {
        if (node->node_type == AST_INC || node->node_type == AST_DEC) {
            char* op_symbol = node->node_type == AST_INC ? "++" : "--";
            if (node->is_prefix) {
                IOStreamWritef(stream, op_symbol);
                compileASTExpression(node->child->left, st, stream, os, false);
            } else {
                compileASTExpression(node->child->left, st, stream, os, false);
                IOStreamWritef(stream, op_symbol);
            }
        } else {
            assert(node->node_type == AST_LOGICAL_TOGGLE || node->node_type == AST_BITWISE_TOGGLE);

            if (node->is_prefix) {
                compileASTExpression(node->child, st, stream, os, false);
            } else {
                if (os->condAssignNeedsTmp() && is_stmt) {
                    writeTmpVar(node->child->value_type, stream);
                    IOStreamWritef(stream, " = ");
                }
                char* op_symbol = node->node_type == AST_LOGICAL_TOGGLE ? "!" : (node->child->value_type == AST_TYPE_BOOL ? "!" : "~");
                IOStreamWritef(stream, "%s(", op_symbol);
                compileASTExpression(node->child, st, stream, os, false);
                IOStreamWritef(stream, ")");
            }
        }
    } else {
        if (node->is_prefix) {
            compileAssignment(node->child, st, os, stream, is_stmt);
        } else {
            ASTNode* new_node = copyAST(node->child);
            ASTResult res;
            switch (node->node_type) {
                case AST_INC:            res = newASTAdd(new_node, newASTInt(-1)); break;
                case AST_DEC:            res = newASTAdd(new_node, newASTInt(1));  break;
                case AST_LOGICAL_TOGGLE: res = newASTLogicalNot(new_node); break;
                case AST_BITWISE_TOGGLE: res = newASTBitwiseNot(new_node); break;
                default:
                    assert(false);
            }
            assert(isOK(res));
            new_node = res.result_value;
            compileASTExpression(new_node, st, stream, os, false);
            deleteASTNode(&new_node);
        }
    }
}

void compileIDDeclaration(Symbol* var, const ASTNode* value, const SymbolTable* st, const IOStream* stream, const OutSerializer* os) {
    assert(var != NULL &&  st != NULL);

    os->parseType(stream, getVarType(var), false);

    IOStreamWritef(stream, " ");

    printId(stream, var, os->print_redef_level);

    if(value != NULL) {
        IOStreamWritef(stream, " = ");
        compileASTExpression(value, st, stream, os, true);
    }
}

static inline const char* compare(const ASTNodeType node_type) {
    switch (node_type) {
        case AST_CMP_EQ:  return "==";
        case AST_CMP_NEQ: return "!=";
        case AST_CMP_LT:  return  "<";
        case AST_CMP_LTE: return "<=";
        case AST_CMP_GT:  return  ">";
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
        compileASTExpression(ast->left, st, stream, os, false);
    }

    IOStreamWritef(stream, " %s ", compare(ast->node_type));

    if(is_chained) {
        if(needsTempVar(ast->right->node_type)) {
            IOStreamWritef(stream, "(");
            writeTmpVar(ast->right->value_type, stream);
            IOStreamWritef(stream, " = ");
            compileASTExpression(ast->right, st, stream, os, false);
            IOStreamWritef(stream, ") && ");
            writeTmpVar(ast->right->value_type, stream);
        } else {
            compileASTExpression(ast->right, st, stream, os, false);
            IOStreamWritef(stream, " && ");
            compileASTExpression(ast->right, st, stream, os, false);
        }
    } else {
        compileASTExpression(ast->right, st, stream, os, false);
    }
}

void compileASTExpression(const ASTNode* node, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, bool is_stmt) {
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
            printId(stream, node->id, os->print_redef_level);
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
            compileChildExpression(node, node->child, st, os, stream);
            break;
        case AST_UADD:
            IOStreamWritef(stream, "+");
            compileChildExpression(node, node->child, st, os, stream);
            break;
        case AST_ABS: {
            IOStreamWritef(stream, "abs(");
            compileASTExpression(node->child, st, stream, os, false);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_POSITIVE: {
            IOStreamWritef(stream, "abs(");
            compileASTExpression(node->child, st, stream, os, false);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_NEGATIVE: {
            IOStreamWritef(stream, "-abs(");
            compileASTExpression(node->child, st, stream, os, false);
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
            compileChildExpression(node, node->child, st, os, stream);
            break;
        case AST_L_SHIFT:
            compileBinaryOP(node, " << ", st, os, stream);
            break;
        case AST_R_SHIFT:
            compileBinaryOP(node, " >> ", st, os, stream);
            break;
        case AST_LOGICAL_NOT:
            IOStreamWritef(stream, "!");
            compileChildExpression(node, node->child, st, os, stream);
            break;
        case AST_LOGICAL_AND:
            compileBinaryOP(node, " && ", st, os, stream);
            break;
        case AST_LOGICAL_OR:
            compileBinaryOP(node, " || ", st, os, stream);
            break;
        case AST_ID_ASSIGNMENT: {
            compileAssignment(node, st, os, stream, is_stmt);
            break;
        } case AST_COMPD_ASSIGN: {
            compileCompoundAssignment(node, st, os, stream, is_stmt);
            break;
        } case AST_INC:
          case AST_DEC:
          case AST_LOGICAL_TOGGLE:
          case AST_BITWISE_TOGGLE: {
            compileUnaryCompoundAssignment(node, st, os, stream, is_stmt);
            break;
        } case AST_TYPE_OF: {
            // TODO: [optimization] it is only worth serializing the exp if it has side-effets. otherwise, only the type matters
            // For that, the type system needs to compute whether an expression is pure

            // We can at least check if child is primitive bool/int/type or ID (trivial cases)

            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            compileASTExpression(node->child, st, s, os, false);
            IOStreamClose(&s);

            os->typeOf(stream, node->child, ptr);
            free(ptr);

            break;
        } case AST_PARENTHESES:
            IOStreamWritef(stream, "(");
            compileASTExpression(node->child, st, stream, os, false);
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
        } case AST_TERNARY_COND: {
            compileChildExpression(node, node->first, st, os, stream);
            IOStreamWritef(stream, " ? ");
            compileChildExpression(node, node->second, st, os, stream);
            IOStreamWritef(stream, " : ");
            compileChildExpression(node, node->third, st, os, stream);
            break;
        } default:
            assert(false);
    }
}

void compileScope(const ASTNode* scope_node, const SymbolTable* st, const IOStream* stream, const OutSerializer* os,
                  unsigned int indentation_level, bool print_new_line) {
    IOStreamWritef(stream, "{\n");

    compileASTStatements(scope_node->child, st, stream, os, indentation_level + 1, true, true);

    indent(stream, indentation_level);
    IOStreamWritef(stream, "}");

    if (print_new_line) {
        IOStreamWritef(stream, "\n");
    }
}

void compileIf(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os,
               unsigned int indentation_level) {
    assert(ast->node_type == AST_IF || ast->node_type == AST_IF_ELSE);

    const ASTNode* cond = ast->node_type == AST_IF ? ast->left : ast->first;
    const ASTNode* then = ast->node_type == AST_IF ? ast->right : ast->second;

    IOStreamWritef(stream, "if (");
    compileASTExpression(cond, st, stream, os, true);
    IOStreamWritef(stream, ") ");

    if (then->node_type == AST_SCOPE) {
        compileScope(then, st, stream, os, indentation_level, false);
    } else {
        IOStreamWritef(stream, "{ ");
        compileASTStatements(then, st, stream, os, 0, false, true);
        IOStreamWritef(stream, " }");
    }

    if (ast->node_type == AST_IF) {
        IOStreamWritef(stream, "\n");
    } else {
        IOStreamWritef(stream, " else ");

        if (ast->third->node_type == AST_IF || ast->third->node_type == AST_IF_ELSE) {
            compileIf(ast->third, st, stream, os, indentation_level);
        } else {
            compileScope(ast->third, st, stream, os, indentation_level, true);
        }
    }
}

void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, unsigned int indentation_level, bool print_new_line, bool print_semicolon) {
    assert(ast != NULL);
    assert(st != NULL);

    if(ast->node_type == AST_STATEMENT_SEQ) {
        compileASTStatements(ast->left, st, stream, os, indentation_level, print_new_line, print_semicolon);
        compileASTStatements(ast->right, st, stream, os, indentation_level, print_new_line, print_semicolon);
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
        } case AST_PRINT: {
            assert(os->print != NULL);
            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            compileASTExpression(ast->child, st, s, os, true);
            IOStreamClose(&s);

            os->print(ptr, ast->child->value_type, NULL, stream);

            free(ptr);
            break;
        } case AST_PRINT_VAR: {
            assert(os->print != NULL);
            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            printId(s, ast->child->id, os->print_redef_level);
            IOStreamClose(&s);

            os->print(ptr, ast->child->value_type, getVarId(ast->child->id), stream);

            free(ptr);
            break;
        }
        case AST_NO_OP: break;
        case AST_SCOPE: {
            compileScope(ast, st, stream, os, indentation_level, true);
            return; // Skip the ;
        }
        case AST_IF: {
            compileIf(ast, st, stream, os, indentation_level);
            return; // Skip the ;
        } 
        case AST_IF_ELSE: {
            compileIf(ast, st, stream, os, indentation_level);
            return; // Skip the ;
        }
        case AST_WHILE: {
            IOStreamWritef(stream, "while (");
            compileASTExpression(ast->left, st, stream, os, true);
            IOStreamWritef(stream, ")");
            if (ast->right->node_type == AST_SCOPE) {
                IOStreamWritef(stream, " ");
                compileScope(ast->right, st, stream, os, indentation_level, true);
            } else if (ast->right->node_type == AST_NO_OP) {
                compileASTStatements(ast->right, st, stream, os, 0, true, true);
            } else {
                assert(false);
            }
            return; // Skip the ;
        }
        case AST_DO_WHILE: {
            IOStreamWritef(stream, "do ");
            if (ast->left->node_type == AST_SCOPE) {
                compileScope(ast->left, st, stream, os, indentation_level, false);
            } else if (ast->left->node_type == AST_NO_OP) {
                IOStreamWritef(stream, "{ }");
            } else {
                assert(false);
            }
            IOStreamWritef(stream, " while (");
            compileASTExpression(ast->right, st, stream, os, true);
            IOStreamWritef(stream, ")");
            break;
        }
        case AST_FOR: {
            const ASTNode* init = ast->child->left;
            const ASTNode* cond = ast->child->right->left;
            const ASTNode* scope = ast->child->right->right;
            assert(scope->node_type == AST_SCOPE);
            const ASTNode* update = scope->child->right;
            const ASTNode* body = scope->child->left;

            IOStreamWritef(stream, "for (");
            compileASTStatements(init, st, stream, os, 0, false, true);
            IOStreamWritef(stream, " ");
            compileASTStatements(cond, st, stream, os, 0, false, true);
            if (update->node_type != AST_NO_OP) {
                IOStreamWritef(stream, " ");
                compileASTStatements(update, st, stream, os, 0, false, false);
            }
            IOStreamWritef(stream, ")");

            if (body->node_type == AST_SCOPE) {
                IOStreamWritef(stream, " ");
                compileScope(body, st, stream, os, indentation_level, true);
            } else if (body->node_type == AST_NO_OP) {
                compileASTStatements(body, st, stream, os, 0, true, true);
            } else {
                assert(false);
            }
            return; // Skip the ;
        }
        default: {
            if(isExp(ast)) {
                compileASTExpression(ast, st, stream, os, true);
            } else {
                assert(false);
            }
            break;
        }
    }

    if(print_semicolon) {
        IOStreamWritef(stream, ";");
    }

    if (print_new_line) {
        IOStreamWritef(stream, "\n");
    }
}
