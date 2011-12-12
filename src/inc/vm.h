#ifndef VM_H
#define VM_H

#include "object.h"

typedef struct IrVM {
    unsigned int        gcstat;
    unsigned int        throttle;
    IrObject           *root_objects;
    IrObject           *sweep_objects;
} IrVM;

#define GC_READY 0
#define GC_SWEEP 1

typedef struct IrState {
} IrState;

IrObject* ir_gc_malloc(IrVM *ir, size_t size);
int ir_gc_mark(IrObject *obj);
int ir_gc_collect(IrVM *vm);

#endif
