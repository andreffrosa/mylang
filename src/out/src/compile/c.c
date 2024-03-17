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

void printC(const IOStream* stream, const char* str, bool printvar) {
    if(printvar) {
        IOStreamWritef(stream, "printf(\"%s = %%d\\n\", %s)", str, str);
    } else {
        IOStreamWritef(stream, "printf(\"%%d\\n\", %s)", str);
    }
}

int outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    assert(ast != NULL && stream != NULL);

    IOStreamWritef(stream, "%s", PRE);
    compileASTStatements(ast, st, stream, &printC, INITIAL_INDENTATION_LEVEL);
    IOStreamWritef(stream, "%s", POS);
}
