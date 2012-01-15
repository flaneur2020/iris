#include "iris.h"


int ir_main(int argc, char **argv) {
    return 0;
}

int ir_test_lex(){
    char *path = "test/sample.lua";
    IrVM vm;
    IrLex lex;
    int tk;

    ir_vm_init(&vm);
    ir_lex_init(&lex, path);
    while((tk = ir_lex_next(&lex)) != 0){
        printf("%s\n", tkstr[tk]);
    }
    return 0;
}

int main(int argc, char **argv) {
    ir_stack_start = get_stack_pointer();
    ir_log("ir_stack_start: %lx\n", ir_stack_start);
    ir_test_lex();
    return 0;
}
