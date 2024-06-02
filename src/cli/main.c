#include <stdbool.h>
#include <string.h>

#include "in/in.h"
#include "out/out.h"

#if defined(_WIN32) || defined(_WIN64)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#define OPEN_FILE_ERR_MSG "Could not open the file %s\n"
#define NO_FILE_ERR_MSG "No file provided!\n"
#define PARSE_AST_ERR_MSG "Error parsing the file %s\n"
#define COMPILE_AST_ERR "Error compiling the file %s\n"
#define COMPILED_MSG "Compiled file %s\n"

void compile(const char* out_file_path_no_ext, size_t len, const char* file_name, const ASTNode* ast, const SymbolTable* st, const char* ext, bool (*compile_to)(const ASTNode* ast, const SymbolTable* st, const char* fname, const IOStream* stream));
bool intrepert(InContext ctx);
static inline void compileFile(const char* file_path);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        InContext ctx = inInitWithStdin();
        while( !(intrepert(ctx)) );
        inDelete(&ctx);
    } else {
        for (int i = 1; i < argc; i++) {
            compileFile(argv[i]);
        }
    }
    return 0;
}

bool intrepert(InContext ctx) {
    printf("> ");

    ASTNode* ast = NULL;
    SymbolTable* st = NULL;
    bool status = inParse(ctx, (ParseContext){&ast, &st, 0});

    if (!status) {
        fprintf(stderr, PARSE_AST_ERR_MSG, "stdin");
        deleteASTNode(&ast);
        deleteSymbolTable(&st);
        return false;
    }

    if(ast == NULL) {
        printf("\n");
        return true; // Exit shell
    }

    printf("Parsed stdin: %d AST nodes.\n", ast->size);

    Frame* frame = executeAST(ast, st);

    IOStream* stream = openIOStreamFromStdout();
    printSymbolTable(st, frame, stream);
    IOStreamClose(&stream);

    deleteFrame(&frame);
    deleteASTNode(&ast);
    deleteSymbolTable(&st);

    return feof(stdin) != 0;
}

void getOutputInfo(const char* file_path, size_t len, char* out_file_path_no_ext, size_t* len_no_ext, const char** file_name) {
    const char* last_dot = strrchr(file_path, '.');
    if(last_dot == file_path) {
        last_dot = NULL;
    }

    size_t chars_to_copy = last_dot != NULL ? (size_t)(last_dot - file_path) : len;
    strncpy(out_file_path_no_ext, file_path, chars_to_copy);
    out_file_path_no_ext[chars_to_copy] = '\0';

    const char* last_slash = strrchr(out_file_path_no_ext, PATH_SEPARATOR);
    *file_name = (last_slash != NULL) ? (last_slash + 1) : file_path;
    *len_no_ext = chars_to_copy;
}

static inline void compileFile(const char* file_path) {
    FILE *in_file = fopen(file_path, "r");
    if (in_file == NULL) {
        fprintf(stderr, OPEN_FILE_ERR_MSG, file_path);
        return;
    }

    InContext ctx = inInitWithFile(in_file);

    ASTNode *ast = NULL;
    SymbolTable* st = NULL;
    bool status = inParse(ctx, (ParseContext){&ast, &st, 0});

    inDelete(&ctx);
    fclose(in_file);

    if (!status) {
        fprintf(stderr, PARSE_AST_ERR_MSG, file_path);
        deleteASTNode(&ast);
        deleteSymbolTable(&st);
        return;
    }

    if(ast == NULL) {
        printf("Nothing to compile!\n");
        return;
    }

    printf("Parsed file %s: %d AST nodes.\n", file_path, ast->size);

    size_t len = strlen(file_path);
    char out_file_path_no_ext[len + 1];
    size_t len_no_ext = 0;
    const char* file_name = NULL;
    getOutputInfo(file_path, len, out_file_path_no_ext, &len_no_ext, &file_name);

    compile(out_file_path_no_ext, len_no_ext, file_name, ast, st, ".c", &outCompileToC);
    compile(out_file_path_no_ext, len_no_ext, file_name, ast, st, ".java", &outCompileToJava);

    deleteASTNode(&ast);
    deleteSymbolTable(&st);
}

void compile(const char* out_file_path_no_ext, size_t len, const char* file_name, const ASTNode* ast, const SymbolTable* st, const char* ext, bool (*compile_to)(const ASTNode* ast, const SymbolTable* st, const char* fname, const IOStream* stream)) {
    size_t ext_len = strlen(ext);
    char out_file_path[len + ext_len + 1];
    strncpy(out_file_path, out_file_path_no_ext, len);
    strncpy(out_file_path + len, ext, ext_len);
    out_file_path[len + ext_len] = '\0';

    FILE* out_file = fopen(out_file_path, "w+");
    if(out_file == NULL) {
        fprintf(stderr, OPEN_FILE_ERR_MSG, out_file_path);
        return;
    }

    IOStream* stream = openIOStreamFromFile(out_file);

    bool status = compile_to(ast, st, file_name, stream);
    IOStreamClose(&stream);
    if(!status) {
        fprintf(stderr, COMPILE_AST_ERR, out_file_path);
        return;
    }

    printf(COMPILED_MSG, out_file_path);
}
