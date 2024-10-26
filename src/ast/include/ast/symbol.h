#ifndef _AST_SYMBOL_H_
#define _AST_SYMBOL_H_

#include <stdbool.h>

#include "utils/iostream.h"

#include "type.h"
#include "errors.h"

#define MAX_ID_SIZE 30

typedef struct Symbol Symbol;
typedef struct Scope Scope;
typedef struct SymbolTable SymbolTable;

SymbolTable* newSymbolTable(unsigned int initial_table_capacity, unsigned int initial_scope_capacity);

SymbolTable* newSymbolTableDefault();

SymbolTable* newSymbolTableClone(const SymbolTable* src_st);

void deleteSymbolTable(SymbolTable** st);

unsigned int getSymbolTableSize(const SymbolTable* st);

unsigned int getSymbolTableCapacity(const SymbolTable* st);

unsigned int getMaxOffset(const SymbolTable* st);

unsigned int getTotalSymbolAmount(const SymbolTable* st);

Scope* enterScope(SymbolTable* st, unsigned int initial_capacity);

Scope* enterScopeDefault(SymbolTable* st);

bool leaveScope(SymbolTable* st);

Scope* getScope(const SymbolTable* st, unsigned int index);

Scope* getCurrentScope(const SymbolTable* st);

unsigned int getScopeSize(const Scope* scope);

unsigned int getScopeCapacity(const Scope* scope);

unsigned int getScopeIndex(const Scope* scope);

Symbol* lookupVar(const SymbolTable *st, const char *id);

Symbol* lookupVarInScope(const Scope* scope, const char *id);

Symbol* lookupLastVarWithOffset(const SymbolTable *st, unsigned int var_offset);

ASTResult defineVar(SymbolTable* st, const ASTType type, const char* id, bool is_init, bool redef);

ASTResult getVarReference(const SymbolTable* st, const char* id, bool is_left);

const char* getVarId(const Symbol* var);

ASTType getVarType(const Symbol* var);

bool isVarInitialized(const Symbol* var);

void setVarInitialized(Symbol* var);

unsigned int getVarOffset(const Symbol* var);

unsigned int getVarRedefLevel(const Symbol* var);

#endif