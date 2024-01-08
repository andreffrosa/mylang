#include "out.h"

#include <string.h>
#include <ctype.h>


static const char* PRE = ""
"    public static void main(String[] args) {\n"
"        int result = ";

static const char* POS = ""
"        System.out.println(result);\n"
"    }\n"
"}\n";

int outCompileToJava(ASTNode* ast, const char* file_name, FILE* out_file) {
    char class_name[strlen(file_name)];
    strcpy(class_name, file_name);
    *class_name = toupper((unsigned char)*class_name);

    fprintf(out_file, "class %s {\n", class_name);
    fprintf(out_file, "%s", PRE);

    outCompileExpression(ast, out_file);
    fprintf(out_file, ";\n");

    fprintf(out_file, "%s", POS);
}
