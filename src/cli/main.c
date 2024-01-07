#include <stdbool.h>

#include "in/in.h"

int main(int argc, char* argv[]) {
    ParseContext ctx = inInitWithStdin();

    int result = 0;
    bool status = false;
    do {
        printf("> ");

        ASTNode* ast = NULL;
        status = inParse(ctx, &ast);
        result = evalAST(ast);

        deleteASTNode(ast);

        printf("= %d (%s)\n", result, status ? "OK" : "ERR");
    } while (true);

    inDelete(&ctx);
}