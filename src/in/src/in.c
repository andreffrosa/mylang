#include "in.h"

#include "ast/ast.h"

#include "parser.h"
#include "lexer.h"

// TODO: clean this file, create tests

struct ParseContext_ {
    yyscan_t scanner;
    YY_BUFFER_STATE input_buffer;
};

#define newParseContext() (malloc(sizeof(struct ParseContext_)))

ParseContext inInitWithFile(FILE* file) {
    struct ParseContext_* ctx = newParseContext();
    yylex_init(&ctx->scanner);
    yyset_in(file, ctx->scanner);
    //yyset_lineno(1, ctx->scanner);
    return ctx;
}

ParseContext inInitWithString(const char* string) {
    struct ParseContext_* ctx = newParseContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = yy_scan_string(string, ctx->scanner);
    yyset_lineno(1, ctx->scanner);
    return ctx;
}

ParseContext inInitWithStdin() {
    struct ParseContext_* ctx = newParseContext();
    yylex_init(&ctx->scanner);
    ctx->input_buffer = NULL;
    return ctx;
}

void inDelete(ParseContext* ctx) {
    if(ctx != NULL && *ctx != NULL) {
        struct ParseContext_* ctx_ = (struct ParseContext_*) *ctx;
        yy_delete_buffer(ctx_->input_buffer, ctx_->scanner);
        yylex_destroy(ctx_->scanner);
        free(ctx_);
        *ctx = NULL;
    }
}

bool inParse(ParseContext ctx, ASTNode** ast) {
    return !yyparse(((struct ParseContext_*) ctx)->scanner, ast);
}

int inLex(ParseContext ctx, void* yylval_param) {
    return yylex((YYSTYPE*)yylval_param, ((struct ParseContext_*) ctx)->scanner);
}

unsigned int inGetLineNumber(ParseContext ctx) {
    return yyget_lineno(((struct ParseContext_*) ctx)->scanner);
}