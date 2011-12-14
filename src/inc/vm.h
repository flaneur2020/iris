#ifndef VM_H
#define VM_H

#include "object.h"
#include "gc.h"

typedef struct IrVM {
    unsigned int        gcstat;
    unsigned int        throttle;
    IrHeap              heaps[HEAPS_MAX];
    unsigned int        heaps_count;
    IrSlot              freeslots;
} IrVM;

#define GC_READY 0
#define GC_SWEEP 1

typedef struct IrState {
    IrVM *vm;
} IrState;

IrObject* ir_gc_malloc(IrVM *ir, size_t size);
int ir_gc_mark(IrObject *obj);
int ir_gc_collect(IrVM *vm);


#endif
