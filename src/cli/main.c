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

void compile(const char* input_file_name, ASTNode* ast, const char* ext, int (*compile_to)(ASTNode* ast, const char* file_name, FILE* out_file));

int main(int argc, char *argv[]) {
    int result = 0;
    bool status = false;

    if (argc == 1) {
        ParseContext ctx = inInitWithStdin();
        do {
            printf("> ");

            ASTNode *ast = NULL;
            status = inParse(ctx, &ast);
            result = evalAST(ast);

            deleteASTNode(&ast);

            printf("= %d (%s)\n", result, status ? "OK" : "ERR");
        } while (true);

        inDelete(&ctx);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *in_file = fopen(argv[i], "r");
            if (in_file == NULL) {
                fprintf(stderr, OPEN_FILE_ERR_MSG, argv[i]);
                return 1;
            }

            ParseContext ctx = inInitWithFile(in_file);
            ASTNode *ast = NULL;
            status = inParse(ctx, &ast);

            inDelete(&ctx);
            fclose(in_file);

            if (!status) {
                fprintf(stderr, "Error parsing the file %s\n", argv[i]);
                deleteASTNode(&ast);
                return 1;
            }

            printf("Parsed file %s: %d AST nodes.\n", argv[i], ast->size);

            result = evalAST(ast);
            printf("= %d (%s)\n", result, status ? "OK" : "ERR");

            compile(argv[i], ast, ".c", &outCompileToC);
            compile(argv[i], ast, ".java", &outCompileToJava);

            deleteASTNode(&ast);
        }
    }

    return 0;
}

void getFileNameWithoutExt(char* file_name, const char* file_path, const char* last_dot, size_t len) {
    const char* last_slash = strrchr(file_path, PATH_SEPARATOR);
    const char* filename_start = (last_slash != NULL) ? (last_slash + 1) : file_path;

    if (last_dot != NULL) {
        strncpy(file_name, filename_start, last_dot - filename_start);
        file_name[last_dot - filename_start] = '\0';
    } else {
        strncpy(file_name, filename_start, len);
    }
}

void getOutFilePath(char* out_file_path, const char* input_file_path, size_t len, const char* last_dot, const char* ext, size_t ext_len) {
    char* ext_start = NULL;
    if (last_dot != NULL) {
        strncpy(out_file_path, input_file_path, last_dot - input_file_path);
        ext_start = out_file_path + (last_dot - input_file_path);
    } else {
        strncpy(out_file_path, input_file_path, len);
        ext_start = out_file_path + len;
    }
    strncpy(ext_start, ext, ext_len);
    *(ext_start + ext_len) = '\0';
}


void compile(const char* input_file_path, ASTNode* ast, const char* ext, int (*compile_to)(ASTNode* ast, const char* fname, FILE* out_file)) {
    const char* last_dot = strrchr(input_file_path, '.');
    size_t len = strlen(input_file_path);

    char file_name[len + 1];
    getFileNameWithoutExt(file_name, input_file_path, last_dot, len);

    size_t ext_len = strlen(ext);
    char out_file_path[len + ext_len + 1];
    getOutFilePath(out_file_path, input_file_path, len, last_dot, ext, ext_len);

    FILE* out_file = fopen(out_file_path, "w+");
    if(out_file == NULL) {
        fprintf(stderr, OPEN_FILE_ERR_MSG, out_file_path);
        return;
    }

    bool status = compile_to(ast, file_name, out_file);
    fclose(out_file);
    if(!status) {
        fprintf(stderr, "Error compiling the file %s\n", input_file_path);
        return;
    }

    printf("Compiled file %s\n", out_file_path);
}
