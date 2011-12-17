#include "inc/iris.h"

/* ------------------------------------------------- */

int ir_vm_init(IrVM *vm) {
    vm->freeslots.next = NULL;
    return 0;
}

int ir_vm_close(IrVM *vm){
    return 0;
}
