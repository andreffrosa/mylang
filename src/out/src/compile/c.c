#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "out.h"

static const char* PRE = ""
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <stdbool.h>\n"
"\n";

static const char* POS = ""
"    return 0;\n"
"}\n";

#define INITIAL_INDENTATION_LEVEL 1

// Lookup table
static const char* ASTTypeCoverter[] = {
    [AST_TYPE_VOID] = "_TYPE_VOID",
    [AST_TYPE_TYPE] = "_TYPE_TYPE",
    [AST_TYPE_INT] = "_TYPE_INT",
    [AST_TYPE_BOOL] = "_TYPE_BOOL",
};

static void print(const char* exp_str, const ASTType type, const char* id_str, const IOStream* stream) {
    IOStreamWritef(stream, "printf(\"");
    if(id_str != NULL) {
        IOStreamWritef(stream, "%s = ", id_str);
    }

    switch (type) {
        case AST_TYPE_VOID:
            IOStreamWritef(stream, "void\\n\"");
            break;
        case AST_TYPE_TYPE:
            IOStreamWritef(stream, "%%s\\n\", _TypeMap[%s])", exp_str);
            break;
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

static const char* parseTypeToStr(const ASTType type, const bool in_exp) {
    if(in_exp) {
        return ASTTypeCoverter[type];
    } else {
        if(type == AST_TYPE_TYPE) {
            return "_Type";
        } else {
            return ASTTypeToStr(type);
        }
    }
}

static void parseType(const IOStream* stream, const ASTType type, const bool in_exp) {
    IOStreamWritef(stream, "%s", parseTypeToStr(type, in_exp));
}

static void typeOf(const IOStream* stream, const ASTNode* node, const char* node_str) {
    const ASTType type = node->value_type;
    const char* str = parseTypeToStr(type, true);
    IOStreamWritef(stream, "(%s, %s)", node_str, str);
}

const OutSerializer cSerializer = {
    &parseType,
    &typeOf,
    &print,
    false
};

static void generateTypeEnum(const IOStream* stream) {
    assert((sizeof(ASTTypeCoverter)/sizeof(ASTTypeCoverter[0])) == AST_TYPE_COUNT);

    IOStreamWritef(stream, "typedef enum _Type {\n");
    for(int i = 0; i < AST_TYPE_COUNT; i++) {
        IOStreamWritef(stream, "    %s,\n", ASTTypeCoverter[i]);
    }
    IOStreamWritef(stream, "} _Type;\n\nconst char* _TypeMap[] = {\n");
    for(int i = 0; i < AST_TYPE_COUNT; i++) {
        IOStreamWritef(stream, "    [%s] = \"%s\",\n", ASTTypeCoverter[i], ASTTypeToStr(i));
    }
    IOStreamWritef(stream, "};\n\n");
}

static void generateTempVars(const IOStream* stream) {
    for(int i = 0; i < AST_TYPE_COUNT; i++) {
        if(i == AST_TYPE_VOID || i == AST_TYPE_TYPE) {
            continue;
        }
        IOStreamWritef(stream, "static %s _tmp_%s;\n", parseTypeToStr(i, false), ASTTypeToStr(i));
    }
    IOStreamWritef(stream, "\n");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool outCompileToC(const ASTNode* ast, const SymbolTable* st, const char* file_name, const IOStream* stream) {
    assert(ast != NULL && stream != NULL);

    IOStreamWritef(stream, "%s", PRE);
    generateTypeEnum(stream);
    generateTempVars(stream);
    //IOStreamWritef(stream, "#define typeof(e, t) (e, t)\n\n");
    IOStreamWritef(stream, "int main(int argc, char** argv) {\n");
    compileASTStatements(ast, st, stream, &cSerializer, INITIAL_INDENTATION_LEVEL, true);
    IOStreamWritef(stream, "%s", POS);

    return true;
}
#pragma GCC diagnostic pop
