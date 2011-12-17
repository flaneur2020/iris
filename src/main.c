#include "inc/iris.h"


int ir_main(int argc, char **argv) {
    char *path = "test/sample.lua";
    IrVM vm;

    ir_vm_init(&vm);
    ir_parse(&vm, path);
    return 0;
}

int main(int argc, char **argv) {
    ir_stack_start = get_stack_pointer();
    ir_log("ir_stack_start: %lx\n", ir_stack_start);
    ir_main(argc, argv);
    return 0;
}
