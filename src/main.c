#include "inc/iris.h"


int ir_main(int argc, char **argv) {
    IrLex lex;
    IrVM *vm;
    int tk;
    char *path = "test/sample.lua";
    int i;
    IrObject *obj;

    vm = ir_vm_new();
    for (i=0; i<100; i++) {
        ir_gc_newobj(vm, T_ARRAY);
    }

    obj = ir_gc_newobj(vm, T_ARRAY);
    ir_gc_mark_stack(vm);
    return 0;

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
    ir_stack_start = get_stack_pointer();
    ir_log("ir_stack_start: %lx\n", ir_stack_start);
    ir_main(argc, argv);
    return 0;
}
