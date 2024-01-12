#include <string.h>
#include <ctype.h>

#include "out.h"

static const char* PRE = ""
"    public static void main(String[] args) {\n"
"        int result = ";

static const char* POS = ""
"        System.out.println(result);\n"
"    }\n"
"}\n";

int outCompileToJava(ASTNode* ast, const char* file_name, IOStream* stream) {
    char class_name[strlen(file_name)];
    strcpy(class_name, file_name);
    *class_name = toupper((unsigned char)*class_name);

    IOStreamWritef(stream, "class %s {\n", class_name);
    IOStreamWritef(stream, "%s", PRE);

    outCompileExpression(ast, stream);
    IOStreamWritef(stream, ";\n");

    IOStreamWritef(stream, "%s", POS);
}
