#include <stdbool.h>

#include "in/in.h"

#define OPEN_FILE_ERR_MSG "Could not open the file %s\n"
#define NO_FILE_ERR_MSG "No file provided!\n"

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

            deleteASTNode(&ast);
        }
    }

    return 0;
}