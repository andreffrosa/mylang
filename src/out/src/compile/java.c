#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "out.h"

static const char* PRE = ""
"    public static void main(String[] args) {\n";

static const char* POS = ""
"    }\n"
"}\n";

#define INITIAL_INDENTATION_LEVEL 2

// Lookup table
static const char* ASTTypeCoverter[] = {
    [AST_TYPE_VOID] = "VOID",
    [AST_TYPE_TYPE] = "TYPE",
    [AST_TYPE_INT] = "INT",
    [AST_TYPE_BOOL] = "BOOL",
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void print(const char* exp_str, const ASTType type, const char* id_str, const IOStream* stream) {
    IOStreamWritef(stream, "System.out.println(");
    if(id_str != NULL) {
        IOStreamWritef(stream, "\"%s = \" + ", id_str);
    }

    if(type == AST_TYPE_TYPE) {
        IOStreamWritef(stream, "%s.nameLowerCase())", exp_str);
    } else {
        IOStreamWritef(stream, "%s)", exp_str);
    }
}
#pragma GCC diagnostic pop

static void parseType(const IOStream* stream, const ASTType type, const bool in_exp) {
    if(in_exp) {
        IOStreamWritef(stream, "_Type.%s", ASTTypeCoverter[type]);
    } else {
        switch (type) {
            case AST_TYPE_BOOL:
                IOStreamWritef(stream, "boolean");
                break;
            case AST_TYPE_TYPE:
                IOStreamWritef(stream, "_Type");
                break;
            default:
                IOStreamWritef(stream, "%s", ASTTypeToStr(type));
                break;
        }
    }
}

static void typeOf(const IOStream* stream, const ASTNode* node, const char* node_str) {
    const ASTType type = node->value_type;

    IOStreamWritef(stream, "(%s", node_str);
    switch (node->value_type) {
    case AST_TYPE_INT:
        IOStreamWritef(stream, " > 0");
        break;
    case AST_TYPE_BOOL:
        break;
    case AST_TYPE_TYPE:
        IOStreamWritef(stream, " == _Type.VOID");
        break;
    case AST_TYPE_VOID:
        assert(false);
        break;
    default:
        assert(false);
    }

    char* ptr = NULL;
    size_t size = 0;
    IOStream* s = openIOStreamFromMemmory(&ptr, &size);
    parseType(s, type, true);
    IOStreamClose(&s);

    IOStreamWritef(stream, " ? %s : %s)", ptr, ptr);
    free(ptr);
}

const OutSerializer javaSerializer = {
    &parseType,
    &typeOf,
    &print,
    true
};

static void printClassName(const IOStream* stream, const char* file_name) {
    char class_name[strlen(file_name)];
    strcpy(class_name, file_name);
    *class_name = toupper((unsigned char)*class_name);

    IOStreamWritef(stream, "class %s {\n", class_name);
}

static void generateTypeEnum(const IOStream* stream) {
    assert((sizeof(ASTTypeCoverter)/sizeof(ASTTypeCoverter[0])) == AST_TYPE_COUNT);

    IOStreamWritef(stream, "\n");
    indent(stream, 1);
    IOStreamWritef(stream, "enum _Type {\n");
    for(int i = 0; i < AST_TYPE_COUNT - 1; i++) {
        indent(stream, 2);
        IOStreamWritef(stream, "%s,\n", ASTTypeCoverter[i]);
    }
    indent(stream, 2);
    IOStreamWritef(stream, "%s;\n\n", ASTTypeCoverter[AST_TYPE_COUNT - 1]);

    indent(stream, 2);
    IOStreamWritef(stream, "public String nameLowerCase() {\n");
    indent(stream, 3);
    IOStreamWritef(stream, "return name().toLowerCase();\n");
    indent(stream, 2);
    IOStreamWritef(stream, "}\n");
    indent(stream, 1);
    IOStreamWritef(stream, "}\n\n");
}

static void generateTempVars(const IOStream* stream) {
    for(int i = 0; i < AST_TYPE_COUNT; i++) {
        if(i == AST_TYPE_VOID || i == AST_TYPE_TYPE) {
            continue;
        }
        indent(stream, 1);
        IOStreamWritef(stream, "private static ");
        parseType(stream, i, false);
        IOStreamWritef(stream, " _tmp_%s;\n", ASTTypeToStr(i));
    }
}

bool outCompileToJava(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    printClassName(stream, file_name);
    generateTypeEnum(stream);
    generateTempVars(stream);
    IOStreamWritef(stream, "\n%s", PRE);
    compileASTStatements(ast, st, stream, &javaSerializer, INITIAL_INDENTATION_LEVEL, true);
    IOStreamWritef(stream, "%s", POS);

    return true;
}

