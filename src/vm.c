#include "iris.h"

/* ------------------------------------------------- */

int ir_vm_init(IrVM *vm) {
    vm->freeslots.next = NULL;
    vm->protos = iv_new(sizeof(IrProto), 16);
    return 0;
}

int ir_vm_close(IrVM *vm){
    iv_free(vm->protos);
    return 0;
}
