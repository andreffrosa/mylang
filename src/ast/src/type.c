#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "type.h"

// Lookup table
const char* ASTTypeMap[] = {
    [AST_TYPE_VOID] = "void",
    [AST_TYPE_TYPE] = "type",
    [AST_TYPE_INT]  = "int",
    [AST_TYPE_BOOL] = "bool",
};

ASTResult typeFromStr(const char* type_str) {
    assert(type_str != NULL);

    for(int i = 0; i < AST_TYPE_COUNT; i ++) {
        if(strcmp(ASTTypeMap[i], type_str) == 0) {
            return OK((ASTType)i);
        }
    }

    return ERR_VAL(AST_RES_ERR_UNKNOWN_TYPE, type_str);
}

const char* ASTTypeValueToStr(const ASTType type, const int value, char* buffer) {
    switch (type) {
        case AST_TYPE_VOID:
            snprintf(buffer, TYPE_VALUE_BUFFER_SIZE, "void");
            return buffer;
        case AST_TYPE_TYPE:
            assert(value < AST_TYPE_COUNT);
            snprintf(buffer, TYPE_VALUE_BUFFER_SIZE, ASTTypeToStr(value));
            return buffer;
        case AST_TYPE_INT:
            snprintf(buffer, TYPE_VALUE_BUFFER_SIZE, "%d", value);
            return buffer;
        case AST_TYPE_BOOL:
            snprintf(buffer, TYPE_VALUE_BUFFER_SIZE, "%s", value ? "true" : "false");
            return buffer;
        default:
            assert(false);
            break;
    }
}
