#include "in.h"

#include "parser.h"
#include "lexer.h"

typedef struct InContext_ {
    yyscan_t scanner;
    YY_BUFFER_STATE input_buffer;
} InContext_;

#define $(in_ctx) (((struct InContext_*) in_ctx))

#define newInContext() (malloc(sizeof(struct InContext_)))

InContext inInitWithFile(FILE* file) {
    InContext_* ctx = newInContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = NULL;
    yyset_in(file, ctx->scanner);
    //yyset_lineno(1, ctx->scanner);
    return ctx;
}

InContext inInitWithString(const char* string) {
    InContext_* ctx = newInContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = yy_scan_string(string, ctx->scanner);
    //yyset_lineno(1, ctx->scanner);
    return ctx;
}

InContext inInitWithStdin() {
    InContext_* ctx = newInContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = NULL;
    return ctx;
}

void inDelete(InContext* in_ctx) {
    if(in_ctx != NULL && *in_ctx != NULL) {
        InContext_* in_ctx_ = $(*in_ctx);
        yy_delete_buffer(in_ctx_->input_buffer, in_ctx_->scanner);
        yylex_destroy(in_ctx_->scanner);
        free(in_ctx_);
        *in_ctx = NULL;
    }
}

bool inParse(InContext in_ctx, ParseContext parse_ctx) {
    if(parse_ctx.ast == NULL) {
        return false;
    }
    if(parse_ctx.st != NULL && *(parse_ctx.st) == NULL) {
        *(parse_ctx.st) = newSymbolTableDefault();
    }
    return !yyparse($(in_ctx)->scanner, parse_ctx);
}

int inLex(InContext in_ctx, void* yylval_param) {
    return yylex((YYSTYPE*)yylval_param, $(in_ctx)->scanner);
}

unsigned int inGetLineNumber(InContext in_ctx) {
    return yyget_lineno($(in_ctx)->scanner);
}
