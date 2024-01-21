#include <assert.h>

#include "out.h"

static const char* PRE = ""
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"\n"
"int main(int argc, char** argv) {\n";

static const char* POS = ""
"    return 0;\n"
"}\n";

#define INITIAL_INDENTATION_LEVEL 1

int outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    assert(ast != NULL && stream != NULL);

    IOStreamWritef(stream, "%s", PRE);
    compileASTStatements(ast, st, stream, INITIAL_INDENTATION_LEVEL);
    IOStreamWritef(stream, "%s", POS);
}
