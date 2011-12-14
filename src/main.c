#include "inc/iris.h"

int ir_main(int argc, char **argv) {
    IrLex lex;
    int tk;
    char *path = "test/sample.lua";

    ir_lex_init(&lex, path);
    while ((tk = ir_lex_next(&lex)) > 0) {
        if (TKSTR(tk)[0] == '<') 
            printf("%s %s\n", TKSTR(tk), lex.buf);
        else 
            printf("%s \n", TKSTR(tk));
    }
    
    return 0;
}

int main(int argc, char **argv) {
    printf("stack pointer: %lu", get_stack_pointer());
    ir_main(argc, argv);
    return 0;
}
