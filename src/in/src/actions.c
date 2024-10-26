#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "actions.h"

static inline void printError(int lineno, const char* type, const char * s, va_list args) {
  fflush(stdout);

  fprintf(stderr, "[%s ERROR] (line %d): ", type, lineno);
  vfprintf(stderr, s, args);
  fprintf(stderr, "\n");
}

void syntaxError(int lineno, const char* s, ...) {
    va_list args;
    va_start(args, s);

    vsyntaxError(lineno, s, args);

    va_end(args);
}

void vsyntaxError(int lineno, const char* s, va_list args) {
    printError(lineno, "SYNTAX", s, args);
}

void semanticError(int lineno, const char* s, ...) {
    va_list args;
    va_start(args, s);

    printError(lineno, "SEMANTIC", s, args);

    va_end(args);
}

ASTNode* handleErrors(ASTResult res, int lineno) {
    if(isOK(res)) {
        return (ASTNode*) res.result_value;
    }

    switch (res.result_type) {
        case AST_RES_ERR_ID_ALREADY_DEFINED: {
            const char* id = res.result_value;
            assert(id != NULL);
            semanticError(lineno, "ID %s was already defined!", id);
            break;
        }
        case AST_RES_ERR_ID_NOT_DEFINED: {
            const char* id = res.result_value;
            assert(id != NULL);
            semanticError(lineno, "Undefined ID %s reference!", id);
            break;
        }
        case AST_RES_ERR_ID_NOT_INIT: {
            const char* id = res.result_value;
            assert(id != NULL);
            semanticError(lineno, "Var %s was not initialized!", id);
            break;
        }
        case AST_RES_ERR_DIFFERENT_TYPES: {
            ASTNode* ast = (ASTNode*) res.result_value;
            assert(ast != NULL);
            semanticError(lineno, "Different types %s and %s!", ASTTypeToStr(ast->left->value_type), ASTTypeToStr(ast->right->value_type));
            deleteASTNode(&ast);
            break;
        }
        case AST_RES_ERR_UNKNOWN_TYPE: {
            const char* type_str = res.result_value;
            assert(type_str != NULL);
            semanticError(lineno, "Unknown type %s!", type_str);
            break;
        }
        case AST_RES_ERR_INVALID_TYPE: {
            const char* type_str = res.result_value;
            assert(type_str != NULL);
            semanticError(lineno, "Variables cannot have type %s!", type_str);
            break;
        }
        case AST_RES_ERR_INVALID_CHILD_TYPE: {
            ASTNode* ast = (ASTNode*) res.result_value;
            assert(ast != NULL);
            assert(getNodeOpType(ast->node_type) == UNARY_OP);
            semanticError(lineno, "Invalid child type %s!", ASTTypeToStr(ast->child->value_type));
            deleteASTNode(&ast);
            break;
        }
        case AST_RES_ERR_INVALID_LEFT_TYPE: {
            ASTNode* ast = (ASTNode*) res.result_value;
            assert(ast != NULL);
            assert(getNodeOpType(ast->node_type) == BINARY_OP);
            semanticError(lineno, "Invalid left type %s!", ASTTypeToStr(ast->left->value_type));
            deleteASTNode(&ast);
            break;
        }
        case AST_RES_ERR_INVALID_RIGHT_TYPE: {
            ASTNode* ast = (ASTNode*) res.result_value;
            assert(ast != NULL);
            assert(getNodeOpType(ast->node_type) == BINARY_OP);
            semanticError(lineno, "Invalid right type %s!", ASTTypeToStr(ast->right->value_type));
            deleteASTNode(&ast);
            break;
        }
        case AST_RES_ERR_NON_TRANSITIVE_CHAINING: {
            ASTNode* ast = (ASTNode*) res.result_value;
            assert(ast != NULL);
            assert(getNodeOpType(ast->node_type) == BINARY_OP);
            semanticError(lineno, "Non-transitive chaining of operators %s and %s!", nodeTypeToStr(ast->left->node_type), nodeTypeToStr(ast->node_type));
            deleteASTNode(&ast);
            break;
        }
        case AST_RES_ERR_UNKNOWN_QUALIFIER: {
            const char* qualifier = res.result_value;
            assert(qualifier != NULL);
            semanticError(lineno, "Unknown qualifier %s!", qualifier);
            break;
        }
        default: {
            semanticError(lineno, "Unknown error %s!", ASTResultTypeToStr(res.result_type));
            assert(false);
            break;
        }
    }

    return NULL;
}

/*
ASTResult declaration(const char* type_str, const char* id, ASTNode* exp, const char* qualifier, SymbolTable* st) {
    assert(type_str != NULL);
    assert(id != NULL);
    assert(qualifier != NULL);

    bool redef;
    if(strlen(qualifier) == 0) {
        redef = false;
    } else if (strcmp("redef", qualifier) == 0) {
        redef = true;
    } else {
        return ERR_VAL(AST_RES_ERR_UNKNOWN_QUALIFIER, qualifier);
    }

    ASTType type;
    if (exp != NULL && strncmp("var", type_str, 3) == 0) {
        type = exp->value_type;
    } else {
        ASTResult res = parseASTType(type_str);
        if (isERR(res)) {
            return res;
        }
        type = (ASTType)res.result_value;
    }

    return newASTIDDeclaration(type, id, exp, redef, st);
}*/

ASTResult declaration(const char* type_str, const char* id, ASTNode* exp, const char* qualifier, SymbolTable* st) {
    assert(type_str != NULL);
    assert(id != NULL);
    assert(qualifier != NULL);

    bool redef;
    if(strlen(qualifier) == 0) {
        redef = false;
    } else if (strcmp("redef", qualifier) == 0) {
        redef = true;
    } else {
        return ERR_VAL(AST_RES_ERR_UNKNOWN_QUALIFIER, qualifier);
    }

    ASTType type;
    if (exp != NULL && strncmp("var", type_str, 3) == 0) {
        type = exp->value_type;
    } else {
        ASTResult res = parseASTType(type_str);
        if (isERR(res)) {
            return res;
        }
        type = (ASTType)res.result_value;
    }

    return newASTIDDeclaration(type, id, exp, redef, st);
}

ASTResult typeFromStr(const char* type_str) {
    ASTResult res = parseASTType(type_str);
    if (isERR(res)) {
        return res;
    }
    ASTType type = (ASTType) res.result_value;
    return OK(newASTType(type));
}

ASTResult handlePrintVar(const char* id, SymbolTable* st) {
    ASTResult res = newASTIDReference(id, st);
    if(isERR(res)) {
        return res;
    }
    ASTNode* id_node = (ASTNode*) res.result_value;
    return OK(newASTPrintVar(id_node));
}