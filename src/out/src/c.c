
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

int outCompileToC(ASTNode* ast, FILE* out_file) {
    fprintf(out_file, "%s", PRE);

    outCompileExpression(ast, out_file);
    fprintf(out_file, ";\n");

    fprintf(out_file, "%s", POS);
}
