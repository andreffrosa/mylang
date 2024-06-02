#include <assert.h>

#include "in.h"

#include "parser.h"
#include "lexer.h"

typedef struct InContext {
    yyscan_t scanner;
    YY_BUFFER_STATE input_buffer;
} InContext;

#define newInContext() (malloc(sizeof(struct InContext)))

InContext* inInitWithFile(FILE* file) {
    assert(file != NULL);
    InContext* ctx = newInContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = NULL;
    yyset_in(file, ctx->scanner);
    return ctx;
}

InContext* inInitWithString(const char* string) {
    assert(string != NULL);
    InContext* ctx = newInContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = yy_scan_string(string, ctx->scanner);
    yyset_lineno(1, ctx->scanner);
    return ctx;
}

InContext* inInitWithStdin() {
    InContext* ctx = newInContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = NULL;
    return ctx;
}

void inDelete(InContext** ctx) {
    assert(ctx != NULL);
    if(*ctx != NULL) {
        yy_delete_buffer((*ctx)->input_buffer, (*ctx)->scanner);
        yylex_destroy((*ctx)->scanner);
        free(*ctx);
        *ctx = NULL;
    }
}

ParseResult inParse(const InContext* ctx) {
    return inParseWithSt(ctx, newSymbolTableDefault());
}

ParseResult inParseWithSt(const InContext* ctx, SymbolTable* st) {
    assert(st != NULL);

    ASTNode* ast = NULL;
    
    ParseContext parse_ctx = {
        .ast = &ast,
        .st = st,
        .nested_comment_level = 0
    };
    
    bool status = !yyparse(ctx->scanner, parse_ctx);

    return (ParseResult){
        .status = status,
        .ast = ast,
        .st = st
    };
}

int inLex(const InContext* ctx, void* yylval_param) {
    unsigned int nested_comment_level = 0;
    return yylex((YYSTYPE*)yylval_param, ctx->scanner, &nested_comment_level);
}

unsigned int inGetLineNumber(const InContext* ctx) {
    return yyget_lineno(ctx->scanner);
}
