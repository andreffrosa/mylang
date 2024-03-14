#include <string.h>
#include <ctype.h>

#include "out.h"

static const char* PRE = ""
"    public static void main(String[] args) {\n";

static const char* POS = ""
"    }\n"
"}\n";

#define INITIAL_INDENTATION_LEVEL 2

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void print(const char* exp_str, const ASTType type, const bool is_printvar, const IOStream* stream) {
    IOStreamWritef(stream, "System.out.println(");
    if(is_printvar) {
        IOStreamWritef(stream, "\"%s = \" + ", exp_str);
    }

    IOStreamWritef(stream, "%s)", exp_str);
}
#pragma GCC diagnostic pop

static const char* parseType(const ASTType type) {
 switch (type) {
    case AST_TYPE_BOOL:
        return "boolean";
    default:
        return ASTTypeToStr(type);
    }
}

const OutSerializer javaSerializer = {
    &parseType,
    &print
};

void setClassName(char* class_name, const char* file_name) {
    strcpy(class_name, file_name);
    *class_name = toupper((unsigned char)*class_name);
}

bool outCompileToJava(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    char class_name[strlen(file_name)];
    setClassName(class_name, file_name);

    IOStreamWritef(stream, "class %s {\n", class_name);
    IOStreamWritef(stream, "%s", PRE);
    compileASTStatements(ast, st, stream, &javaSerializer, INITIAL_INDENTATION_LEVEL);
    IOStreamWritef(stream, "%s", POS);

    return true;
}
