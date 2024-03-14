#include <assert.h>

#include "out.h"

static const char* PRE = ""
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <stdbool.h>\n"
"\n"
"int main(int argc, char** argv) {\n";

static const char* POS = ""
"    return 0;\n"
"}\n";

#define INITIAL_INDENTATION_LEVEL 1

static void print(const char* exp_str, const ASTType type, const bool is_printvar, const IOStream* stream) {
    IOStreamWritef(stream, "printf(\"");
    if(is_printvar) {
        IOStreamWritef(stream, "%s = ", exp_str);
    }

    switch (type) {
        case AST_TYPE_INT:
            IOStreamWritef(stream, "%%d\\n\", %s)", exp_str);
            break;
        case AST_TYPE_BOOL:
            IOStreamWritef(stream, "%%s\\n\", (%s ? \"true\" : \"false\"))", exp_str);
            break;
        default:
            assert(false);
    }
}

static const char* parseType(const ASTType type) {
    return ASTTypeToStr(type);
}

const OutSerializer cSerializer = {
    &parseType,
    &print
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    assert(ast != NULL && stream != NULL);

    IOStreamWritef(stream, "%s", PRE);
    compileASTStatements(ast, st, stream, &cSerializer, INITIAL_INDENTATION_LEVEL);
    IOStreamWritef(stream, "%s", POS);

    return true;
}
#pragma GCC diagnostic pop