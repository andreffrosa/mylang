#include <stdarg.h>
#include <assert.h>

#include "actions.h"

void printError(int lineno, const char * s, ...) {
  va_list ap;
  va_start(ap, s);

  fflush(stdout);

  fprintf(stderr, "(line %d) PARSE ERROR: ", lineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

ASTNode* declaration(const char* id, SymbolTable* st, int lineno) {
    ASTResult res = newASTIDDeclaration(id, st);
    if (isERR(res)) {
        if(res.type == ASTResult_ERR_ID_ALREADY_DEFINED) {
            printError(lineno, "ID %s was already defined!", id);
            return NULL;
        } else {
            assert(false);
        }
    }
    assert(!isVarInitialized(res.ast->child->id));
    return res.ast;
}

ASTNode* declarationAssignment(const char* id, ASTNode* exp, SymbolTable* st, int lineno) {
    ASTResult res = newASTIDDeclarationAssignment(id, exp, st);
    if (isERR(res)) {
        if(res.type == ASTResult_ERR_ID_ALREADY_DEFINED) {
            printError(lineno, "ID %s was already defined!", id);
            return NULL;
        } else {
            assert(false);
        }
    }
    assert(isVarInitialized(res.ast->left->id));
    return res.ast;
}

ASTNode* assignment(const char* id, ASTNode* exp, SymbolTable* st, int lineno) {
    ASTResult res = newASTAssignment(id, exp, st);
    if (isERR(res)) {
        if(res.type == ASTResult_ERR_ID_NOT_DEFINED) {
            printError(lineno, "Undefined ID %s reference!", id);
            return NULL;
        } else {
            assert(false);
        }
    }
    assert(isVarInitialized(res.ast->left->id));
    return res.ast;
}

ASTNode* idReference(const char* id, SymbolTable* st, int lineno) {
    ASTResult res = newASTIDReference(id, st);
    if (isERR(res)) {
        if(res.type == ASTResult_ERR_ID_NOT_DEFINED) {
            printError(lineno, "Undefined ID %s reference!", id);
            return NULL;
        } else if(res.type == ASTResult_ERR_ID_NOT_INIT) {
            printError(lineno, "Var %s was not initialized!", id);
            return NULL;
        } else {
            assert(false);
        }
    }
    assert(isVarInitialized(res.ast->id));
    return res.ast;
}