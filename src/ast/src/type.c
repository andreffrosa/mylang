#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "error.h"

#include "type.h"

// Lookup table
const char* ASTTypeMap[] = {
    [AST_TYPE_VOID] = "void",
    [AST_TYPE_TYPE] = "type",
    [AST_TYPE_INT] = "int",
};

ASTResult parseASTType(const char* type_str) {
    assert(type_str != NULL);

    for(int i = 0; i < AST_TYPE_COUNT; i ++) {
        if(strcmp(ASTTypeMap[i], type_str) == 0) {
            return OK((ASTType)i);
        }
    }

    return ERR(AST_RES_ERR_INVALID_CHILD_TYPE);
}

const char* ASTTypeValueToStr(const ASTType type, const int value, char* buffer) {
    switch (type) {
        case AST_TYPE_VOID:
            return "void";
        case AST_TYPE_TYPE:
            return ASTTypeToStr(value);
        case AST_TYPE_INT:
            snprintf(buffer, TYPE_VALUE_BUFFER_SIZE, "%d", value);
            return buffer;
        default:
            assert(false);
            break;
    }
}
