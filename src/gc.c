#include "inc/iris.h"

/* ------------------------------------------------- */

/* 
 * allocate memory from vm.
 * */
IrObject* ir_gc_malloc(IrVM *vm, size_t size) {
    IrObject *obj;
    
    obj = (IrObject*)malloc(size);
}

int ir_gc_free(IrObject *obj){
}

int ir_gc_mark(IrObject *obj) {
}

int ir_gc_collect(IrVM *vm) {
}
