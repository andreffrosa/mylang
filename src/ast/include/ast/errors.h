#ifndef _AST_ERROR_H_
#define _AST_ERROR_H_

typedef enum ASTResultType {
    AST_RES_OK = 0,
    AST_RES_ERR_ID_ALREADY_DEFINED,
    AST_RES_ERR_ID_NOT_DEFINED,
    AST_RES_ERR_ID_NOT_INIT,
    AST_RES_ERR_UNKNOWN_TYPE,
    AST_RES_ERR_INVALID_TYPE,
    AST_RES_ERR_DIFFERENT_TYPES,
    AST_RES_ERR_INVALID_CHILD_TYPE,
    AST_RES_ERR_INVALID_LEFT_TYPE,
    AST_RES_ERR_INVALID_RIGHT_TYPE,
    AST_RES_ERR_NON_TRANSITIVE_CHAINING,
    AST_RES_ERR_UNKNOWN_QUALIFIER,
    AST_RES_TYPE_COUNT
} ASTResultType;

typedef struct ASTResult {
    ASTResultType result_type;
    void* result_value;
} ASTResult;

#define OK(V) (ASTResult){.result_type = AST_RES_OK, .result_value = (void*) V}

#define ERR(T) (ASTResult){.result_type = T, .result_value = NULL}

#define ERR_VAL(T, V) (ASTResult){.result_type = T, .result_value = (void*) V}

#define isOK(res) (res.result_type == AST_RES_OK)

#define isERR(res) !isOK(res)

extern const char* ASTResultTypeMap[];

#define ASTResultTypeToStr(result_type) (result_type >= AST_RES_TYPE_COUNT ? NULL : ASTResultTypeMap[result_type])

#endif
