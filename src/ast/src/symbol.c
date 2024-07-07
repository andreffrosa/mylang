#include <assert.h>
#include <string.h>

#include "symbol.h"

#define DEFAULT_INITIAL_CAPACITY 10

typedef struct SymbolTable {
    unsigned int size;
    unsigned int capacity;
    Symbol** variables;
} SymbolTable;

typedef struct Symbol {
    char id[MAX_ID_SIZE];
    unsigned int index;
    bool is_init:1;
    ASTType type;
} Symbol;

SymbolTable* newSymbolTable(unsigned int initial_capacity) {
    assert(initial_capacity > 0);
    SymbolTable* st = malloc(sizeof(SymbolTable));
    st->variables = malloc(initial_capacity * sizeof(Symbol*));
    st->capacity = initial_capacity;
    st->size = 0;
    return st;
}

SymbolTable* newSymbolTableDefault() {
    return newSymbolTable(DEFAULT_INITIAL_CAPACITY);
}

SymbolTable* newSymbolTableClone(const SymbolTable* src_st) {
    assert(src_st != NULL);
    SymbolTable* st_clone = malloc(sizeof(SymbolTable));
    st_clone->variables = malloc(src_st->capacity * sizeof(Symbol*));
    st_clone->capacity = src_st->capacity;
    st_clone->size = src_st->size;
    for(unsigned int i = 0; i < src_st->size; i++) {
        st_clone->variables[i] = malloc(sizeof(Symbol));
        memcpy(st_clone->variables[i], src_st->variables[i], sizeof(Symbol));
    }
    return st_clone;
}

void deleteSymbolTable(SymbolTable** st) {
    assert(st != NULL && *st != NULL);
    if((*st)->variables != NULL) {
        for(unsigned int i = 0; i < (*st)->size; i++){
            free((*st)->variables[i]);
        }
        free((*st)->variables);
    }
    free(*st);
    *st = NULL;
}

static inline void resizeTableIfNeeded(SymbolTable* st) {
    if(st->size == st->capacity) {
        st->capacity *= 2;
        st->capacity++;
        st->variables = realloc(st->variables, st->capacity * sizeof(Symbol*));
    }
}

unsigned int getSymbolTableSize(const SymbolTable* st) {
    assert(st != NULL);
    return st->size;
}

unsigned int getSymbolTableCapacity(const SymbolTable* st) {
    assert(st != NULL);
    return st->capacity;
}

Symbol* newSymbol() {
    return malloc(sizeof(Symbol));
}

Symbol* initSymbol(Symbol* var, const ASTType type, const char* id, unsigned int index, const bool is_init) {
    assert(var != NULL);
    var->type = type;
    strncpy(var->id, id, MAX_ID_SIZE);
    var->index = index;
    var->is_init = is_init;
    return var;
}

Symbol* insertVar(SymbolTable* st, const ASTType type, const char* id) {
    assert(st != NULL && id != NULL);

    assert(type != AST_TYPE_VOID);

    resizeTableIfNeeded(st);

    int index = st->size++;
    st->variables[index] = initSymbol(newSymbol(), type, id, index, false);
    return st->variables[index];
}

Symbol* lookupVar(const SymbolTable* st, const char* id) {
    assert(st != NULL && id != NULL);

    for(unsigned int i = 0; i < st->size; i++) {
        Symbol* var = st->variables[i];
        if(strncmp(var->id, id, MAX_ID_SIZE) == 0) {
            return var;
        }
    }
    return NULL;
}

unsigned int getVarIndex(const SymbolTable* st, const Symbol* var) {
    assert(st != NULL && var != NULL);
    return var->index;
}


const char* getVarId(const Symbol* var) {
    assert(var != NULL);
    return var->id;
}

ASTType getVarType(const Symbol* var) {
    assert(var != NULL);
    return var->type;
}

bool isVarInitialized(const Symbol* var) {
    assert(var != NULL);
    return var->is_init;
}

void setVarInitialized(Symbol* var) {
    assert(var != NULL);
    var->is_init = true;
}

Symbol* getVarFromIndex(const SymbolTable* st, unsigned int index) {
    assert(st != NULL);
    if(index >= st->size) {
        return NULL;
    }
    return st->variables[index];
}