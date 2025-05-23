#ifndef _AST_TYPE_H_
#define _AST_TYPE_H_

#include "errors.h"

typedef enum ASTType {
    AST_TYPE_VOID = 0,  // No type
    AST_TYPE_TYPE,      // Types are also a type
    AST_TYPE_INT,
    AST_TYPE_BOOL,
    AST_TYPE_COUNT,
} ASTType;

extern const char* ASTTypeMap[];

ASTResult typeFromStr(const char* type_str);

#define ASTTypeToStr(type) (type >= AST_TYPE_COUNT ? "" : ASTTypeMap[type])

#define TYPE_VALUE_BUFFER_SIZE (8 * sizeof(int) + 1)

const char* ASTTypeValueToStr(const ASTType type, const int value, char* buffer);

#endif
