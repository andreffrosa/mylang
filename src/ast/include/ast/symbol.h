#ifndef _AST_SYMBOL_H_
#define _AST_SYMBOL_H_

#include <stdbool.h>

#include "utils/iostream.h"

#include "type.h"

#define MAX_ID_SIZE 30

typedef struct Symbol Symbol;

typedef struct SymbolTable SymbolTable;

SymbolTable* newSymbolTable(unsigned int initial_capacity);

SymbolTable* newSymbolTableDefault();

SymbolTable* newSymbolTableClone(const SymbolTable* src_st);

void deleteSymbolTable(SymbolTable** st);

unsigned int getSymbolTableSize(const SymbolTable* st);

unsigned int getSymbolTableCapacity(const SymbolTable* st);

Symbol* insertVar(SymbolTable* st, const ASTType type, const char* id);

Symbol* lookupVar(const SymbolTable* st, const char* id);

#define checkIDWasNotDeclared(id, st) (lookupVar(st, id) == NULL)

const char* getVarId(const Symbol* var);

ASTType getVarType(const Symbol* var);

bool isVarInitialized(const Symbol* var);

void setVarInitialized(Symbol* var);

unsigned int getVarIndex(const SymbolTable* st, const Symbol* var);

Symbol* getVarFromIndex(const SymbolTable* st, unsigned int index);

#endif