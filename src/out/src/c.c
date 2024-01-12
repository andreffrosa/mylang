
#include "out.h"

static const char* PRE = ""
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"\n"
"int main(int argc, char** argv) {\n"
"    int result = ";

static const char* POS = ""
"    printf(\"%d\\n\", result);\n"
"    return 0;\n"
"}\n";

int outCompileToC(ASTNode* ast, const char* file_name, IOStream* stream) {
    IOStreamWritef(stream, "%s", PRE);

    outCompileExpression(ast, stream);
    IOStreamWritef(stream, ";\n");

    IOStreamWritef(stream, "%s", POS);
}
