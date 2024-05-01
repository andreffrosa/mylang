#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "actions.h"

void printError(int lineno, const char * s, ...) {
  va_list ap;
  va_start(ap, s);

  fflush(stdout);

  fprintf(stderr, "(line %d) PARSE ERROR: ", lineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

ASTNode* declaration(const char* type_str, const char* id, SymbolTable* st, int lineno) {
    ASTResult res = parseASTType(type_str);
    if (isERR(res)) {
        printError(lineno, "Unknown type %s!", type_str);
        return NULL;
    }

    ASTType type = (ASTType) res.result_value;
    res = newASTIDDeclaration(type, id, st);
    if (isERR(res)) {
        switch (res.result_type) {
            case AST_RES_ERR_ID_ALREADY_DEFINED:
                printError(lineno, "ID %s was already defined!", id);
                return NULL;
            case AST_RES_ERR_INVALID_CHILD_TYPE:
                printError(lineno, "ID %s has invalid type %s!", id, ASTTypeToStr(type));
                return NULL;
            default:
                printError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
                assert(false);
        }
    }
    ASTNode* ast = res.result_value;
    assert(!isVarInitialized(ast->child->id));
    return ast;
}

ASTNode* declarationAssignment(const char* type_str, const char* id, ASTNode* exp, SymbolTable* st, int lineno) {
    ASTType type;
    if(strncmp("var", type_str, 3) == 0) {
        type = exp->value_type;
    } else {
        ASTResult res = parseASTType(type_str);
        if (isERR(res)) {
            printError(lineno, "Unknown type %s!", type_str);
            return NULL;
        }
        type = (ASTType) res.result_value;
    }

    ASTResult res = newASTIDDeclarationAssignment(type, id, exp, st);
    if (isERR(res)) {
        switch (res.result_type) {
            case AST_RES_ERR_ID_ALREADY_DEFINED:
                printError(lineno, "ID %s was already defined!", id);
                return NULL;
            case AST_RES_ERR_INVALID_LEFT_TYPE:
                printError(lineno, "ID %s has invalid type %s!", id, ASTTypeToStr(type));
                return NULL;
            default:
                printError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
                assert(false);
        }
    }
    ASTNode* ast = res.result_value;
    assert(isVarInitialized(ast->left->id));
    return ast;
}

ASTNode* assignment(const char* id, ASTNode* exp, SymbolTable* st, int lineno) {
    ASTResult res = newASTAssignment(id, exp, st);
    if (isERR(res)) {
        if(res.result_type == AST_RES_ERR_ID_NOT_DEFINED) {
            printError(lineno, "Undefined ID %s reference!", id);
            return NULL;
        } else {
            printError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
            assert(false);
        }
    }
    ASTNode* ast = res.result_value;
    assert(isVarInitialized(ast->left->id));
    return ast;
}

ASTNode* idReference(const char* id, SymbolTable* st, int lineno) {
    ASTResult res = newASTIDReference(id, st);
    if (isERR(res)) {
        if(res.result_type == AST_RES_ERR_ID_NOT_DEFINED) {
            printError(lineno, "Undefined ID %s reference!", id);
            return NULL;
        } else if(res.result_type == AST_RES_ERR_ID_NOT_INIT) {
            printError(lineno, "Var %s was not initialized!", id);
            return NULL;
        } else {
            printError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
            assert(false);
        }
    }
    ASTNode* ast = res.result_value;
    assert(isVarInitialized(ast->id));
    return ast;
}

ASTNode* binaryOp(ASTResult res, ASTNode* left, ASTNode* right, int lineno) {
    if (isERR(res)) {
        if(res.result_type == AST_RES_ERR_INVALID_LEFT_TYPE) {
            printError(lineno, "Invalid left type %s!", ASTTypeToStr(left->value_type));
            return NULL;
        } else if(res.result_type == AST_RES_ERR_INVALID_RIGHT_TYPE) {
            printError(lineno, "Invalid right type %s!", ASTTypeToStr(right->value_type));
            return NULL;
        } else {
            printError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
            assert(false);
        }
    }
    return (ASTNode*) res.result_value;
}

ASTNode* unaryOp(ASTResult res, ASTNode* child, int lineno) {
    if (isERR(res)) {
        if(res.result_type == AST_RES_ERR_INVALID_CHILD_TYPE) {
            printError(lineno, "Invalid child type %s!", ASTTypeToStr(child->value_type));
            return NULL;
        } else {
            printError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
            assert(false);
        }
    }
    return (ASTNode*) res.result_value;
}