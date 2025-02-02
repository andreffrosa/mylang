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
        case AST_PARENTHESES:
            return 14;
        case AST_INC:
        case AST_DEC:
        //    return node->is_prefix ? 13 : 14;
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
            printf("Precedence not defined for %s\n", nodeTypeToStr(node_type));
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


static inline void compileBinaryOP(const ASTNode* node, const char* op_symbol, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool is_rval) {
    bool need_parentheses = needParentheses(node->node_type, node->left->node_type, true);
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->left, st, stream, os, is_rval);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }

    IOStreamWritef(stream, op_symbol);

    need_parentheses = needParentheses(node->node_type, node->right->node_type, false);
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->right, st, stream, os, is_rval);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }
}

void compileTernaryCond(const ASTNode* node, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool is_rval) {
    bool need_parentheses = node->first->node_type == AST_TERNARY_COND;
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->first, st, stream, os, is_rval);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }

    IOStreamWritef(stream, " ? ");

    need_parentheses = node->second->node_type == AST_TERNARY_COND;
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->second, st, stream, os, is_rval);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }

    IOStreamWritef(stream, " : ");

    need_parentheses = node->third->node_type == AST_TERNARY_COND;
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    compileASTExpression(node->third, st, stream, os, is_rval);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }
}

static void writeTmpVar(const ASTType type, const IOStream* stream) {
    const char* tmp_var_type = ASTTypeToStr(type);
    IOStreamWritef(stream, "_tmp_%s", tmp_var_type);
}

static void compileLVal(const ASTNode* lval, const char* op_symbol, const char* rval, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool first, bool is_rval) {
    assert(lval != NULL);
    assert(rval != NULL);

    if (first && os->condAssignNeedsTmp() && lval->node_type != AST_ID && !is_rval) {
        writeTmpVar(lval->value_type, stream);
        IOStreamWritef(stream, " = ");
    }

    bool need_parentheses = (is_rval || !first) && lval->node_type != AST_PARENTHESES;
    if (need_parentheses) { IOStreamWritef(stream, "("); }

    switch (lval->node_type) {
        case AST_ID: {
            Symbol* var = lval->id;
            IOStreamWritef(stream, "%s %s %s", getVarId(var), op_symbol, rval);
        } break;
        case AST_TERNARY_COND: {
            // Alternative in C: *(cond ? &a : &b) = rval
            compileASTExpression(lval->first, st, stream, os, is_rval);
            IOStreamWritef(stream, " ? ");
            compileLVal(lval->second, op_symbol, rval, st, os, stream, false, false);
            IOStreamWritef(stream, " : ");
            compileLVal(lval->third, op_symbol, rval, st, os, stream, false, false);
        } break;
        case AST_PARENTHESES: {
            compileLVal(lval->child, op_symbol, rval, st, os, stream, false, false);
        } break;
        default:
            assert(false);
            break;
    }

    if (need_parentheses) { IOStreamWritef(stream, ")"); }
}

void compileAssignment(const ASTNode* lval, const ASTNode* rval, const char* op_symbol, const SymbolTable* st, const OutSerializer* os, const IOStream* stream, bool is_rval) {
    char* ptr = NULL;
    size_t size = 0;
    IOStream* s = openIOStreamFromMemmory(&ptr, &size);
    compileASTExpression(rval, st, s, os, true);
    IOStreamClose(&s);

    compileLVal(lval, op_symbol, ptr, st, os, stream, true, is_rval);

    free(ptr);
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

void compileCmpExp(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, const bool is_chained, bool is_rval) {
    assert(stream != NULL);

    if(isCmpExp(ast->left)) { // left is chained
        compileCmpExp(ast->left, st, stream, os, true, is_rval);
    } else {
        compileASTExpression(ast->left, st, stream, os, is_rval);
    }

    IOStreamWritef(stream, " %s ", compare(ast->node_type));

    if(is_chained) {
        if(needsTempVar(ast->right->node_type)) {
            IOStreamWritef(stream, "(");
            writeTmpVar(ast->right->value_type, stream);
            IOStreamWritef(stream, " = ");
            compileASTExpression(ast->right, st, stream, os, is_rval);
            IOStreamWritef(stream, ") && ");
            writeTmpVar(ast->right->value_type, stream);
        } else {
            compileASTExpression(ast->right, st, stream, os, is_rval);
            IOStreamWritef(stream, " && ");
            compileASTExpression(ast->right, st, stream, os, is_rval);
        }
    } else {
        compileASTExpression(ast->right, st, stream, os, is_rval);
    }
}

void compileASTExpression(const ASTNode* node, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, bool is_rval) {
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
            compileBinaryOP(node, " + ", st, os, stream, is_rval);
            break;
        case AST_SUB:
            compileBinaryOP(node, " - ", st, os, stream, is_rval);
            break;
        case AST_MUL:
            compileBinaryOP(node, "*", st, os, stream, is_rval);
            break;
        case AST_DIV:
            compileBinaryOP(node, "/", st, os, stream, is_rval);
            break;
        case AST_MOD:
            compileBinaryOP(node, "%%", st, os, stream, is_rval);
            break;
        case AST_USUB:
            IOStreamWritef(stream, "-");
            compileASTExpression(node->child, st, stream, os, is_rval);
            break;
        case AST_UADD:
            IOStreamWritef(stream, "+");
            compileASTExpression(node->child, st, stream, os, is_rval);
            break;
        case AST_ABS: {
            IOStreamWritef(stream, "abs(");
            compileASTExpression(node->child, st, stream, os, is_rval);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_POSITIVE: {
            IOStreamWritef(stream, "abs(");
            compileASTExpression(node->child, st, stream, os, is_rval);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_NEGATIVE: {
            IOStreamWritef(stream, "-abs(");
            compileASTExpression(node->child, st, stream, os, is_rval);
            IOStreamWritef(stream, ")");
            break;
        } case AST_BITWISE_AND:
            compileBinaryOP(node, "&", st, os, stream, is_rval);
            break;
        case AST_BITWISE_OR:
            compileBinaryOP(node, "|", st, os, stream, is_rval);
            break;
        case AST_BITWISE_XOR:
            compileBinaryOP(node, "^", st, os, stream, is_rval);
            break;
        case AST_BITWISE_NOT:
            if(node->child->value_type == AST_TYPE_BOOL) {
                IOStreamWritef(stream, "!");
            } else {
                IOStreamWritef(stream, "~");
            }
            compileASTExpression(node->child, st, stream, os, is_rval);
            break;
        case AST_L_SHIFT:
            compileBinaryOP(node, " << ", st, os, stream, is_rval);
            break;
        case AST_R_SHIFT:
            compileBinaryOP(node, " >> ", st, os, stream, is_rval);
            break;
        case AST_LOGICAL_NOT:
            IOStreamWritef(stream, "!");
            compileASTExpression(node->child, st, stream, os, is_rval);
            break;
        case AST_LOGICAL_AND:
            compileBinaryOP(node, " && ", st, os, stream, is_rval);
            break;
        case AST_LOGICAL_OR:
            compileBinaryOP(node, " || ", st, os, stream, is_rval);
            break;
        case AST_ID_ASSIGNMENT: {
            compileAssignment(node->left, node->right, "=", st, os, stream, is_rval);
            break;
        } case AST_INC: {
            const ASTNode* lval = node->child->left;
            if (lval->node_type == AST_ID) {
                if (node->is_prefix) {
                    IOStreamWritef(stream, "++");
                    compileASTExpression(lval, st, stream, os, is_rval);
                } else {
                    compileASTExpression(lval, st, stream, os, is_rval);
                    IOStreamWritef(stream, "++");
                }
            } else {
                ASTNode* aux = copyAST(node->child);
                if (node->is_prefix) {
                    compileAssignment(aux->left, aux->right, "=", st, os, stream, is_rval);
                } else {
                    ASTResult res = newASTAdd(aux, newASTInt(-1));
                    assert(isOK(res));
                    aux = res.result_value;
                    compileASTExpression(aux, st, stream, os, is_rval);
                }
                deleteASTNode(&aux);
            }
            break;
        } case AST_DEC: {
            const ASTNode* lval = node->child->left;
            if (lval->node_type == AST_ID) {
                if (node->is_prefix) {
                    IOStreamWritef(stream, "--");
                    compileASTExpression(lval, st, stream, os, is_rval);
                } else {
                    compileASTExpression(lval, st, stream, os, is_rval);
                    IOStreamWritef(stream, "--");
                }
            } else {
                ASTNode* aux = copyAST(node->child);
                if (node->is_prefix) {
                    compileAssignment(aux->left, aux->right, "=", st, os, stream, is_rval);
                } else {
                    ASTResult res = newASTAdd(aux, newASTInt(1));
                    assert(isOK(res));
                    aux = res.result_value;
                    compileASTExpression(aux, st, stream, os, is_rval);
                }
                deleteASTNode(&aux);
            }
            break;
        } case AST_LOGICAL_TOGGLE: {
            const ASTNode* lval = node->child->left;
            if (lval->node_type == AST_ID) {
                if (node->is_prefix) {
                    compileASTExpression(node->child, st, stream, os, is_rval);
                } else {
                    if (os->condAssignNeedsTmp() && !is_rval) {
                        writeTmpVar(node->child->value_type, stream);
                        IOStreamWritef(stream, " = ");
                    }
                    IOStreamWritef(stream, "!(");
                    compileASTExpression(node->child, st, stream, os, is_rval);
                    IOStreamWritef(stream, ")");
                }
            } else {
                ASTNode* aux = copyAST(node->child);
                if (node->is_prefix) {
                    compileAssignment(aux->left, aux->right, "=", st, os, stream, is_rval);
                } else {
                    ASTResult res = newASTLogicalNot(aux);
                    assert(isOK(res));
                    aux = res.result_value;
                    compileASTExpression(aux, st, stream, os, is_rval);
                }
                deleteASTNode(&aux);
            }
            break;
        } case AST_BITWISE_TOGGLE: {
            const ASTNode* lval = node->child->left;
            if (lval->node_type == AST_ID) {
                if (node->is_prefix) {
                    compileASTExpression(node->child, st, stream, os, is_rval);
                } else {
                    if (os->condAssignNeedsTmp() && !is_rval) {
                        writeTmpVar(node->child->value_type, stream);
                        IOStreamWritef(stream, " = ");
                    }
                    IOStreamWritef(stream, (node->child->value_type == AST_TYPE_BOOL ? "!(" : "~("));
                    compileASTExpression(node->child, st, stream, os, is_rval);
                    IOStreamWritef(stream, ")");
                }
            } else {
                ASTNode* aux = copyAST(node->child);
                if (node->is_prefix) {
                    compileAssignment(aux->left, aux->right, "=", st, os, stream, is_rval);
                } else {
                    ASTResult res = newASTBitwiseNot(aux);
                    assert(isOK(res));
                    aux = res.result_value;
                    compileASTExpression(aux, st, stream, os, is_rval);
                }
                deleteASTNode(&aux);
            }
            break;
        } case AST_COMPD_ASSIGN: {
            const ASTNode* op_node = node->child->right;

            if (!os->hasCompdAssign(op_node->node_type)) {
                compileASTExpression(node->child, st, stream, os, is_rval);
                break;
            }

            char* op_symbol = NULL;
            switch (op_node->node_type) {
                case AST_ADD: op_symbol = "+="; break;
                case AST_SUB: op_symbol = "-="; break;
                case AST_MUL: op_symbol = "*="; break;
                case AST_DIV: op_symbol = "/="; break;
                case AST_MOD: op_symbol = "%="; break;
                case AST_BITWISE_AND: op_symbol = "&="; break;
                case AST_BITWISE_OR: op_symbol = "|="; break;
                case AST_BITWISE_XOR: op_symbol = "^="; break;
                case AST_L_SHIFT: op_symbol = "<<="; break;
                case AST_R_SHIFT: op_symbol = ">>="; break;
                case AST_LOGICAL_AND: op_symbol = "&&="; break;
                case AST_LOGICAL_OR: op_symbol = "||="; break;
                default:
                    assert(false);
            }

            compileAssignment(node->child->left, op_node->right, op_symbol, st, os, stream, is_rval);

            break;
        } case AST_TYPE_OF: {
            // TODO: [optimization] it is only worth serializing the exp if it has side-effets. otherwise, only the type matters
            // For that, the type system needs to compute whether an expression is pure

            // We can at least check if child is primitive bool/int/type or ID (trivial cases)

            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            compileASTExpression(node->child, st, s, os, is_rval);
            IOStreamClose(&s);

            os->typeOf(stream, node->child, ptr);
            free(ptr);
            break;
        } case AST_PARENTHESES:
            IOStreamWritef(stream, "(");
            compileASTExpression(node->child, st, stream, os, is_rval);
            IOStreamWritef(stream, ")");
            break;
        case AST_CMP_EQ:
        case AST_CMP_NEQ:
        case AST_CMP_LT:
        case AST_CMP_LTE:
        case AST_CMP_GT:
        case AST_CMP_GTE: {
            compileCmpExp(node, st, stream, os, false, is_rval);
            break;
        }
        case AST_TERNARY_COND: {
            compileTernaryCond(node, st, os, stream, is_rval);
            break;
        }
        default:
            assert(false);
    }
}

void compileScope(const ASTNode* scope_node, const SymbolTable* st, const IOStream* stream, const OutSerializer* os,
                  unsigned int indentation_level, bool print_new_line) {
    IOStreamWritef(stream, "{\n");

    compileASTStatements(scope_node->child, st, stream, os, indentation_level + 1, true);

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
        compileASTStatements(then, st, stream, os, 0, false);
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

void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, const OutSerializer* os, unsigned int indentation_level, bool print_new_line) {
    assert(ast != NULL);
    assert(st != NULL);

    if(ast->node_type == AST_STATEMENT_SEQ) {
        compileASTStatements(ast->left, st, stream, os, indentation_level, print_new_line);
        compileASTStatements(ast->right, st, stream, os, indentation_level, print_new_line);
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
        default: {
            if(isExp(ast)) {
                compileASTExpression(ast, st, stream, os, false);
            } else {
                assert(false);
            }
            break;
        }
    }

    if (print_new_line) {
        IOStreamWritef(stream, ";\n");
    } else {
        IOStreamWritef(stream, ";");
    }
}
