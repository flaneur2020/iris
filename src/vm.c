#include "inc/iris.h"

/* ------------------------------------------------- */

IrVM* ir_vm_new() {
    IrVM *vm;
    
    vm = malloc(sizeof(IrVM));
    memset(vm, 0, sizeof(IrVM));
    vm->gcstat = GC_READY;
    vm->freeslots.next = NULL;
    return vm;
}

int ir_vm_free(IrVM *vm){
    free(vm);
    return 0;
}
