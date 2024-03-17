#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ast/ast.h"

#include "out.h"

#define DEFAULT_IDENTATION "    "

#define ERR_UNDEFINED_ID "[ERROR]: Undefined ID %s !\n"
#define ERR_DECLARED_ID "[ERROR]: ID %s already declared!\n"
#define ERR_ALLOC_ID "[ERROR]: Couldn't allocate var %s!\n"

static inline void indent(const IOStream* stream, unsigned int indentation_level) {
    for(int i = 0; i < indentation_level; i++) {
        IOStreamWritef(stream, DEFAULT_IDENTATION);
    }
}

static inline int getPrecedence(ASTNodeType node_type) {
    switch (node_type) {
        case AST_NUMBER:
        case AST_ID:
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


static inline void compileBinaryOP(const ASTNode* node, const char* op_symbol, const SymbolTable* st, const IOStream* stream) {
    bool need_parentheses = needParentheses(node->type, node->left->type, true);
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    outCompileExpression(node->left, st, stream);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }

    IOStreamWritef(stream, op_symbol);

    need_parentheses = needParentheses(node->type, node->right->type, false);
    if(need_parentheses) {
        IOStreamWritef(stream, "(");
    }
    outCompileExpression(node->right, st, stream);
    if(need_parentheses) {
        IOStreamWritef(stream, ")");
    }
}

int outCompileExpression(const ASTNode* node, const SymbolTable* st, const IOStream* stream) {
    assert(node != NULL);

    switch (node->type) {
        case AST_NUMBER:
            IOStreamWritef(stream, "%d", node->n);
            break;
        case AST_ID:
            IOStreamWritef(stream, "%s", node->id);
            break;
        case AST_ADD:
            compileBinaryOP(node, " + ", st, stream);
            break;
        case AST_SUB:
            compileBinaryOP(node, " - ", st, stream);
            break;
        case AST_MUL:
            compileBinaryOP(node, "*", st, stream);
            break;
        case AST_DIV:
            compileBinaryOP(node, "/", st, stream);
            break;
        case AST_MOD:
            compileBinaryOP(node, "%%", st, stream);
            break;
        case AST_USUB:
            IOStreamWritef(stream, "-");
            outCompileExpression(node->child, st, stream);
            break;
        case AST_UADD:
            IOStreamWritef(stream, "+");
            outCompileExpression(node->child, st, stream);
            break;
        case AST_ABS: {
            IOStreamWritef(stream, "abs(");
            outCompileExpression(node->child, st, stream);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_POSITIVE: {
            IOStreamWritef(stream, "abs(");
            outCompileExpression(node->child, st, stream);
            IOStreamWritef(stream, ")");
            break;
        } case AST_SET_NEGATIVE: {
            IOStreamWritef(stream, "-abs(");
            outCompileExpression(node->child, st, stream);
            IOStreamWritef(stream, ")");
            break;
        } case AST_BITWISE_AND:
            compileBinaryOP(node, "&", st, stream);
            break;
        case AST_BITWISE_OR:
            compileBinaryOP(node, "|", st, stream);
            break;
        case AST_BITWISE_XOR:
            compileBinaryOP(node, "^", st, stream);
            break;
        case AST_BITWISE_NOT:
            IOStreamWritef(stream, "~");
            outCompileExpression(node->child, st, stream);
            break;
        case AST_L_SHIFT:
            compileBinaryOP(node, " << ", st, stream);
            break;
        case AST_R_SHIFT:
            compileBinaryOP(node, " >> ", st, stream);
            break;
        default:
            assert(false);
    }
}

void compileIDDeclaration(Symbol* var, const ASTNode* value, const SymbolTable* st, const IOStream* stream) {
    assert(var != NULL &&  st != NULL);

    IOStreamWritef(stream, "int %s", getVarId(var));

    if(value != NULL) {
        IOStreamWritef(stream, " = ", getVarId(var));
        outCompileExpression(value, st, stream);
    }
}

void compileASTStatements(const ASTNode* ast, const SymbolTable* st, const IOStream* stream, printFunc print, unsigned int indentation_level) {
    assert(ast != NULL && st != NULL);

    if(ast->type == AST_STATEMENT_SEQ) {
        compileASTStatements(ast->left, st, stream, print, indentation_level);
        compileASTStatements(ast->right, st, stream, print, indentation_level);
        return;
    }

    indent(stream, indentation_level);
    switch (ast->type) {
        case AST_ID_DECLARATION: {
            assert(ast->child->type == AST_ID);
            Symbol* var = ast->child->id;
            compileIDDeclaration(var, NULL, st, stream);
            break;
        } case AST_ID_DECL_ASSIGN: {
            assert(ast->left->type == AST_ID);
            Symbol* var = ast->left->id;
            compileIDDeclaration(var, ast->right, st, stream);
            break;
        } case AST_ID_ASSIGNMENT: {
            assert(ast->left->type == AST_ID);
            Symbol* var = ast->left->id;
            IOStreamWritef(stream, "%s = ", getVarId(var));
            outCompileExpression(ast->right, st, stream);
            break;
        } case AST_PRINT:
          case AST_PRINT_VAR: {
            assert(print != NULL);
            char* ptr;
            size_t size;
            IOStream* s = openIOStreamFromMemmory(&ptr, &size);
            outCompileExpression(ast->child, st, s);
            IOStreamClose(&s);

            print(stream, ptr, ast->type == AST_PRINT_VAR);

            free(ptr);
            break;
        } default: {
            if(isExp(ast)) {
                outCompileExpression(ast, st, stream);
            } else {
                assert(false);
            }
            break;
        }
    }
    IOStreamWritef(stream, ";\n");
}
