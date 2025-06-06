#ifndef _AST_H_
#define _AST_H_

#include <stdbool.h>

#include "errors.h"
#include "symbol.h"
#include "type.h"

typedef enum ASTNodeType {
    AST_INT,
    AST_BOOL,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_MOD,
    AST_USUB,
    AST_UADD,
    AST_BITWISE_OR,
    AST_BITWISE_AND,
    AST_BITWISE_XOR,
    AST_BITWISE_NOT,
    AST_L_SHIFT,
    AST_R_SHIFT,
    AST_ABS,
    AST_SET_POSITIVE,
    AST_SET_NEGATIVE,
    AST_LOGICAL_NOT,
    AST_LOGICAL_AND,
    AST_LOGICAL_OR,
    AST_ID,
    AST_ID_DECLARATION,
    AST_ID_DECL_ASSIGN,
    AST_ID_ASSIGNMENT,
    AST_STATEMENT_SEQ,
    AST_PRINT,
    AST_PRINT_VAR,
    AST_NO_OP,
    AST_TYPE,
    AST_TYPE_OF,
    AST_PARENTHESES,
    AST_CMP_EQ,
    AST_CMP_NEQ,
    AST_CMP_LT,
    AST_CMP_LTE,
    AST_CMP_GT,
    AST_CMP_GTE,
    AST_SCOPE,
    AST_TERNARY_COND,
    AST_IF,
    AST_IF_ELSE,
    AST_INC,
    AST_DEC,
    AST_LOGICAL_TOGGLE,
    AST_BITWISE_TOGGLE,
    AST_COMPD_ASSIGN,
    AST_WHILE,
    AST_DO_WHILE,
    AST_FOR,
    AST_BREAK,
    AST_CONTINUE,
    AST_NODE_TYPES_COUNT  // Count of AST node types
} ASTNodeType;

typedef enum ASTOpType {
    ZEROARY_OP,
    UNARY_OP,
    BINARY_OP,
    TERNARY_OP,
    UNKNOWN_OP
} ASTOpType;

typedef struct ASTNode {
    ASTNodeType node_type;
    ASTType value_type;

    unsigned int size;
    bool allowed_lval;

    union {
        int n;      // AST_INT
        bool z;     // AST_BOOL
        ASTType t;  // AST_TYPE
        Symbol* id; // AST_ID
        struct {    // BINARY_OP
            const struct ASTNode* left;
            const struct ASTNode* right;
        };
        struct {    // UNARY_OP
            const struct ASTNode* child;
            bool is_prefix:1; // inc and dec
        };
        struct {    // TERNARY_OP
            struct ASTNode* first;
            struct ASTNode* second;
            struct ASTNode* third;
        };
    };
} ASTNode;

void deleteASTNode(ASTNode** node);

ASTNode* newASTInt(const int n);

ASTNode* newASTBool(const bool z);

ASTNode* newASTType(const ASTType t);

ASTResult newASTBinaryOP(const ASTNodeType type, const ASTNode* left, const ASTNode* right);

ASTResult newASTUnaryOP(const ASTNodeType type, const ASTNode* child);

ASTResult newASTTernaryOP(ASTNodeType node_type, ASTNode* first, ASTNode* second, ASTNode* third);

ASTNode* newASTNoOp();

bool equalAST(const ASTNode* ast1, const ASTNode* ast2);

bool isStmt(const ASTNode* ast);

bool isExp(const ASTNode* ast);

bool isCmpExp(const ASTNode* ast);

ASTOpType getNodeOpType(const ASTNodeType node_type);

const char* nodeTypeToStr(ASTNodeType node_type);

ASTNode* copyAST(const ASTNode* src_ast);

int printAST(const ASTNode* ast, const IOStream* stream, int level);

#define newASTAdd(l, r) newASTBinaryOP(AST_ADD, l, r)
#define newASTSub(l, r) newASTBinaryOP(AST_SUB, l, r)
#define newASTMul(l, r) newASTBinaryOP(AST_MUL, l, r)
#define newASTDiv(l, r) newASTBinaryOP(AST_DIV, l, r)
#define newASTMod(l, r) newASTBinaryOP(AST_MOD, l, r)

#define newASTUSub(e) newASTUnaryOP(AST_USUB, e)
#define newASTUAdd(e) newASTUnaryOP(AST_UADD, e)

#define newASTBitwiseAnd(l, r) newASTBinaryOP(AST_BITWISE_AND, l, r)
#define newASTBitwiseOr(l, r) newASTBinaryOP(AST_BITWISE_OR, l, r)
#define newASTBitwiseXor(l, r) newASTBinaryOP(AST_BITWISE_XOR, l, r)
#define newASTBitwiseNot(e) newASTUnaryOP(AST_BITWISE_NOT, e)
#define newASTLeftShift(l, r) newASTBinaryOP(AST_L_SHIFT, l, r)
#define newASTRightShift(l, r) newASTBinaryOP(AST_R_SHIFT, l, r)

#define newASTAbs(e) newASTUnaryOP(AST_ABS, e)
#define newASTSetPositive(e) newASTUnaryOP(AST_SET_POSITIVE, e)
#define newASTSetNegative(e) newASTUnaryOP(AST_SET_NEGATIVE, e)

#define newASTLogicalNot(e) newASTUnaryOP(AST_LOGICAL_NOT, e)
#define newASTLogicalAnd(l, r) newASTBinaryOP(AST_LOGICAL_AND, l, r)
#define newASTLogicalOr(l, r) newASTBinaryOP(AST_LOGICAL_OR, l, r)

#define newASTStatementList(stmt, list) (list == NULL ? stmt : newASTBinaryOP(AST_STATEMENT_SEQ, stmt, list).result_value)

ASTNode* newASTID(Symbol* id);
ASTResult newASTIDDeclaration(ASTType type, const char* id, const ASTNode* value, bool redef, SymbolTable* st);
#define newASTAssignment(l, r) newASTBinaryOP(AST_ID_ASSIGNMENT, l, r)
ASTResult newASTUnaryCompoundAssign(ASTNodeType node_type, const ASTNode* lval, bool is_prefix);
ASTResult newASTCompoundAssignment(ASTNodeType node_type, const ASTNode* lval, ASTNode* value);
#define newASTInc(e, p) newASTUnaryCompoundAssign(AST_INC, e, p)
#define newASTDec(e, p) newASTUnaryCompoundAssign(AST_DEC, e, p)
#define newASTLogicalToggle(e, p) newASTUnaryCompoundAssign(AST_LOGICAL_TOGGLE, e, p)
#define newASTBitwiseToggle(e, p) newASTUnaryCompoundAssign(AST_BITWISE_TOGGLE, e, p)

#define newASTPrint(e) newASTUnaryOP(AST_PRINT, e).result_value
#define newASTPrintVar(e) newASTUnaryOP(AST_PRINT_VAR, e).result_value
#define newASTTypeOf(e) newASTUnaryOP(AST_TYPE_OF, e).result_value

ASTNode* newASTParentheses(ASTNode* ast);

#define newASTCmpEQ(l, r) newASTBinaryOP(AST_CMP_EQ, l, r)
#define newASTCmpNEQ(l, r) newASTBinaryOP(AST_CMP_NEQ, l, r)
#define newASTCmpLT(l, r) newASTBinaryOP(AST_CMP_LT, l, r)
#define newASTCmpLTE(l, r) newASTBinaryOP(AST_CMP_LTE, l, r)
#define newASTCmpGT(l, r) newASTBinaryOP(AST_CMP_GT, l, r)
#define newASTCmpGTE(l, r) newASTBinaryOP(AST_CMP_GTE, l, r)

#define newASTScope(s) newASTUnaryOP(AST_SCOPE, s).result_value

#define newASTTernaryCond(e, l, r) newASTTernaryOP(AST_TERNARY_COND, e, l, r)
#define newASTIf(cond, t) newASTBinaryOP(AST_IF, cond, t)
#define newASTIfElse(cond, t, f) newASTTernaryOP(AST_IF_ELSE, cond, t, f)

#define newASTWhile(cond, s) newASTBinaryOP(AST_WHILE, cond, s)
#define newASTDoWhile(s, cond) newASTBinaryOP(AST_DO_WHILE, s, cond)
ASTResult newASTFor(const ASTNode* init, const ASTNode* cond, const ASTNode* update, const ASTNode* body);
ASTNode* newASTBreak();
ASTNode* newASTContinue();

#endif