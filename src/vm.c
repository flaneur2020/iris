#include "inc/iris.h"

/* ------------------------------------------------- */

IrVM* ir_vm_new() {
    IrVM *vm;
    
    vm = malloc(sizeof(IrVM));
    vm->gcstat = GC_READY;
    return vm;
}
