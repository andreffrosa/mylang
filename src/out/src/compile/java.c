#include <string.h>
#include <ctype.h>

#include "out.h"

static const char* PRE = ""
"    public static void main(String[] args) {\n";

static const char* POS = ""
"    }\n"
"}\n";

#define INITIAL_INDENTATION_LEVEL 2

void printJava(const IOStream* stream, const char* str, bool printvar) {
    if(printvar) {
        IOStreamWritef(stream, "System.out.println(\"%s = \" + %s)", str, str);
    } else {
        IOStreamWritef(stream, "System.out.println(%s)", str);
    }
}

void setClassName(char* class_name, const char* file_name) {
    strcpy(class_name, file_name);
    *class_name = toupper((unsigned char)*class_name);
}

int outCompileToJava(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    char class_name[strlen(file_name)];
    setClassName(class_name, file_name);

    IOStreamWritef(stream, "class %s {\n", class_name);
    IOStreamWritef(stream, "%s", PRE);
    compileASTStatements(ast, st, stream, &printJava, INITIAL_INDENTATION_LEVEL);
    IOStreamWritef(stream, "%s", POS);
}
