#include "symbol.h"

#include <assert.h>
#include <string.h>

#define DEFAULT_TABLE_INITIAL_CAPACITY 5
#define DEFAULT_SCOPE_INITIAL_CAPACITY 10

typedef struct Symbol {
    char id[MAX_ID_SIZE];
    ASTType type;
    bool is_init : 1;
    unsigned int offset;
    unsigned int redef_level;
} Symbol;

typedef struct Scope {
    unsigned int index;
    const struct Scope* parent;
    unsigned int size;
    unsigned int capacity;
    unsigned int offset;
    Symbol** variables;
} Scope;

typedef struct SymbolTable {
    Scope** scopes;
    unsigned int size;
    unsigned int capacity;
    Scope* current_scope;
    unsigned int max_offset;
    unsigned int total_symbol_amount;
} SymbolTable;

#define newScope() malloc(sizeof(Scope))

static Scope* initScope(Scope* scope, unsigned int index, const Scope* parent, unsigned int initial_capacity, unsigned int offset) {
    assert(scope != NULL);
    scope->index = index;
    scope->variables = malloc(initial_capacity * sizeof(Symbol*));
    assert(scope->variables != NULL);
    scope->capacity = initial_capacity;
    scope->size = 0;
    scope->parent = parent;
    scope->offset = offset;
    return scope;
}

static void deleteScope(Scope** scope) {
    assert(scope != NULL && *scope != NULL);
    assert((*scope)->variables != NULL);

    for (unsigned int i = 0; i < (*scope)->size; i++) {
        free((*scope)->variables[i]);
    }
    free((*scope)->variables);

    free(*scope);
    *scope = NULL;
}

static inline void resizeScopeIfNeeded(Scope* scope) {
    if (scope->size == scope->capacity) {
        scope->capacity *= 2;
        scope->capacity++;
        scope->variables = realloc(scope->variables, scope->capacity * sizeof(Symbol*));
        assert(scope->variables != NULL);
    }
}

static Scope* insertNewScope(SymbolTable* st, unsigned int initial_capacity) {
    assert(st != NULL && initial_capacity > 0);

    Scope* parent = st->current_scope;
    unsigned int offset = (parent == NULL ? 0 : parent->offset + parent->size);
    unsigned int scope_index = st->size++;
    Scope* scope = initScope(newScope(), scope_index, parent, initial_capacity, offset);
    st->scopes[scope_index] = scope;
    st->current_scope = scope;

    return scope;
}

SymbolTable* newSymbolTable(unsigned int initial_table_capacity, unsigned int initial_scope_capacity) {
    assert(initial_table_capacity > 0 && initial_scope_capacity > 0);

    SymbolTable* st = malloc(sizeof(SymbolTable));
    assert(st != NULL);
    st->scopes = malloc(initial_table_capacity * sizeof(Scope*));
    assert(st->scopes != NULL);
    st->capacity = initial_table_capacity;
    st->size = 0;

    st->current_scope = NULL;
    insertNewScope(st, initial_scope_capacity);
    st->max_offset = 0;

    st->total_symbol_amount = 0;

    assert(st->current_scope != NULL);
    return st;
}

SymbolTable* newSymbolTableDefault() {
    return newSymbolTable(DEFAULT_TABLE_INITIAL_CAPACITY, DEFAULT_SCOPE_INITIAL_CAPACITY);
}

SymbolTable* newSymbolTableClone(const SymbolTable* src_st) {
    assert(src_st != NULL);

    SymbolTable* clone_st = malloc(sizeof(SymbolTable));
    assert(clone_st != NULL);
    clone_st->scopes = malloc(src_st->capacity * sizeof(Scope*));
    assert(clone_st->scopes != NULL);
    clone_st->capacity = src_st->capacity;
    clone_st->size = src_st->size;
    clone_st->max_offset = src_st->max_offset;
    clone_st->current_scope = NULL;
    clone_st->total_symbol_amount = src_st->total_symbol_amount;

    for (unsigned int i = 0; i < src_st->size; i++) {
        Scope* src_scope = src_st->scopes[i];
        Scope* clone_scope = malloc(sizeof(Scope));
        assert(clone_scope != NULL);
        clone_st->scopes[i] = clone_scope;

        if (src_scope == src_st->current_scope) {
            clone_st->current_scope = clone_scope;
        }

        clone_scope->variables = malloc(src_scope->capacity * sizeof(Symbol*));
        assert(clone_scope->variables != NULL);
        clone_scope->capacity = src_scope->capacity;
        clone_scope->size = src_scope->size;

        for (unsigned int j = 0; j < clone_scope->size; j++) {
            clone_scope->variables[j] = malloc(sizeof(Symbol));
            assert(clone_scope->variables[j] != NULL);
            memcpy(clone_scope->variables[j], src_scope->variables[j], sizeof(Symbol));
        }

        if (src_scope->parent == NULL) {
            clone_scope->parent = NULL;
        } else {
            assert(i > 0);
            unsigned int parent_index = getScopeIndex(src_scope->parent);
            assert(src_st->scopes[parent_index] == src_scope->parent);  // was found
            clone_scope->parent = clone_st->scopes[parent_index];
        }
    }

    assert(clone_st->current_scope != NULL);
    return clone_st;
}

void deleteSymbolTable(SymbolTable** st) {
    assert(st != NULL && *st != NULL);
    assert((*st)->scopes != NULL);

    for (unsigned int i = 0; i < (*st)->size; i++) {
        Scope* s = (*st)->scopes[i];
        deleteScope(&s);
    }
    free((*st)->scopes);

    free(*st);
    *st = NULL;
}

static inline void resizeTableIfNeeded(SymbolTable* st) {
    if (st->size == st->capacity) {
        st->capacity *= 2;
        st->capacity++;
        st->scopes = realloc(st->scopes, st->capacity * sizeof(Scope*));
        assert(st->scopes != NULL);
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

unsigned int getMaxOffset(const SymbolTable* st) {
    assert(st != NULL);
    return st->max_offset;
}

unsigned int getTotalSymbolAmount(const SymbolTable* st) {
    assert(st != NULL);
    return st->total_symbol_amount;
}

Scope* enterScope(SymbolTable* st, unsigned int initial_capacity) {
    assert(st != NULL && initial_capacity > 0);

    resizeTableIfNeeded(st);

    return insertNewScope(st, initial_capacity);
}

Scope* enterScopeDefault(SymbolTable* st) {
    return enterScope(st, DEFAULT_SCOPE_INITIAL_CAPACITY);
}

bool leaveScope(SymbolTable* st) {
    assert(st != NULL);
    if(st->current_scope->parent == NULL) {
        return false;
    }
    st->current_scope = (Scope*)st->current_scope->parent;
    return true;
}

Scope* getScope(const SymbolTable* st, unsigned int index) {
    assert(st != NULL);

    if(index >= st->size) {
        return NULL;
    }
    return st->scopes[index];
}

Scope* getCurrentScope(const SymbolTable* st) {
    assert(st != NULL);
    return st->current_scope;
}

unsigned int getScopeSize(const Scope* scope) {
    assert(scope != NULL);
    return scope->size;
}

unsigned int getScopeCapacity(const Scope* scope) {
    assert(scope != NULL);
    return scope->capacity;
}

unsigned int getScopeIndex(const Scope* scope) {
    assert(scope != NULL);
    return scope->index;
}

Symbol* newSymbol() { return malloc(sizeof(Symbol)); }

Symbol* initSymbol(Symbol* var, ASTType type, const char* id, bool is_init, unsigned int redef_level) {
    assert(var != NULL);
    var->type = type;
    strncpy(var->id, id, MAX_ID_SIZE);
    var->offset = 0;
    var->is_init = is_init;
    var->redef_level = redef_level;
    return var;
}

Symbol* lookupVarInScope(const Scope* scope, const char* id) {
    assert(scope != NULL);
    assert(id != NULL);

    for (unsigned int i = 0; i < scope->size; i++) {
        Symbol* var = scope->variables[i];
        if (strncmp(var->id, id, MAX_ID_SIZE) == 0) {
            return var;
        }
    }
    return NULL;
}

struct SymbolScopePair {
    Symbol* symbol;
    Scope* scope;
};
struct SymbolScopePair lookupVarWithScope(const SymbolTable* st, const char* id) {
    assert(st != NULL);
    assert(id != NULL);

    Scope* current_scope = st->current_scope;
    while (current_scope != NULL) {
        Symbol* var = lookupVarInScope(current_scope, id);
        if (var != NULL) {
            return (struct SymbolScopePair){
                .symbol = var,
                .scope = current_scope,
            };
        }
        current_scope = (Scope*)current_scope->parent;
    }
    return (struct SymbolScopePair){
        .symbol = NULL,
        .scope = NULL,
    };
}

Symbol* lookupVar(const SymbolTable* st, const char* id) {
    assert(st != NULL);
    assert(id != NULL);
    return lookupVarWithScope(st, id).symbol;
}

Symbol* lookupLastVarWithOffset(const SymbolTable *st, unsigned int var_offset) {
    assert(st != NULL);

    for (int i = st->size - 1; i >= 0; i--) {
        Scope* current_scope = st->scopes[i];
        if (current_scope->offset <= var_offset) {
            for (unsigned int j = 0; j < current_scope->size; j++) {
                Symbol* var = current_scope->variables[j];
                if (getVarOffset(var) == var_offset) {
                    return var;
                }
            }
        }
    }
    return NULL;
}

static Symbol* insertVarInCurrentScope(SymbolTable* st, ASTType type, const char* id, bool is_init, unsigned int redef_level) {
    Symbol* var = initSymbol(newSymbol(), type, id, is_init, redef_level);

    resizeScopeIfNeeded(st->current_scope);

    unsigned int index = st->current_scope->size++;
    st->current_scope->variables[index] = var;

    var->offset = st->current_scope->offset + index;

    unsigned int current_offset = getVarOffset(var);
    if (current_offset > st->max_offset) {
        st->max_offset = current_offset;
    }

    st->total_symbol_amount++;

    return var;
}

ASTResult defineVar(SymbolTable* st, ASTType type, const char* id, bool is_init, bool redef) {
    assert(st != NULL);
    assert(id != NULL);

    struct SymbolScopePair prev_def = lookupVarWithScope(st, id);
    if (prev_def.symbol != NULL) {
        if (!redef || prev_def.scope == st->current_scope) {  // TODO: split into two different error types???
            return ERR_VAL(AST_RES_ERR_ID_ALREADY_DEFINED, id);
        }

        Symbol* var = insertVarInCurrentScope(st, type, id, is_init, prev_def.symbol->redef_level + 1);
        return OK(var);
    } else {
        if (redef) {
            return ERR_VAL(AST_RES_ERR_ID_NOT_DEFINED, id);
        }

        Symbol* var = insertVarInCurrentScope(st, type, id, is_init, 0);
        return OK(var);
    }
}

ASTResult getVarReference(const SymbolTable* st, const char* id, bool is_left) {
    assert(st != NULL);
    assert(id != NULL);

    Symbol* var = lookupVar(st, id);
    if (var == NULL) {
        return ERR_VAL(AST_RES_ERR_ID_NOT_DEFINED, id);
    }

    if (!is_left && !isVarInitialized(var)) {
        return ERR_VAL(AST_RES_ERR_ID_NOT_INIT, id);
    }

    return OK(var);
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

unsigned int getVarOffset(const Symbol* var) {
    assert(var != NULL);
    return var->offset;
}

unsigned int getVarRedefLevel(const Symbol* var) {
    assert(var != NULL);
    return var->redef_level;
}
