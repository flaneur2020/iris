#ifndef VM_H
#define VM_H

#include "object.h"
#include "gc.h"
#include "proto.h"

typedef struct IrVM {
    struct ir_vector   *protos;
    //
    unsigned int        throttle;
    IrHeap              heaps[HEAPS_MAX];
    unsigned int        heaps_count;
    IrSlot              freeslots;
} IrVM;

typedef struct IrState {
    IrVM *vm;
} IrState;

#endif
