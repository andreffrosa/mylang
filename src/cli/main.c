#include <stdbool.h>
#include <string.h>

#include "in/in.h"
#include "out/out.h"

#define OPEN_FILE_ERR_MSG "Could not open the file %s\n"
#define NO_FILE_ERR_MSG "No file provided!\n"

void compile(const char* input_file_name, ASTNode* ast, const char* ext, int (*compile_to)(ASTNode* ast, FILE* out_file));
int getOutFilePath(const char* input_file_path, size_t len, const char* ext, char* out_file_path);

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

            deleteASTNode(&ast);
        }
    }

    return 0;
}

void compile(const char* input_file_name, ASTNode* ast, const char* ext, int (*compile_to)(ASTNode* ast, FILE* out_file)) {
    size_t len = strlen(input_file_name);
    char out_file_path[len+1];
    getOutFilePath(input_file_name, len, ext, out_file_path);

    FILE* out_file = fopen(out_file_path, "w+");
    if(out_file == NULL) {
        fprintf(stderr, OPEN_FILE_ERR_MSG, out_file_path);
        return;
    }

    bool status = compile_to(ast, out_file);
    fclose(out_file);
    if(!status) {
        fprintf(stderr, "Error compiling the file %s\n", input_file_name);
        return;
    }

    printf("Compiled file %s\n", out_file_path);
}

int getOutFilePath(const char* input_file_path, size_t len, const char* ext, char* out_file_path) {
    strncpy(out_file_path, input_file_path, len+1);
    char* ptr = out_file_path + len;
    while (*ptr != '.') { ptr--;}
    strncpy(ptr, ext, strlen(ext)+1);
    return len;
}