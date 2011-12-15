#ifndef GC_H
#define GC_H

#include "object.h"
#include "vm.h"

typedef struct IrSlot {
    IrObject ohead;
    struct IrSlot *next;
} IrSlot;

typedef union IrSlotBody {
    IrSlot slot;
    IrArray array;
    IrString string;
} IrSlotBody;

typedef struct IrHeap {
    unsigned long start;
    size_t size;
} IrHeap;

#define HEAPS_MAX 16
#define HEAP_DEFAULT_SIZE 4096
#define HEAP_INCREMENT_FACTOR 4

unsigned long ir_stack_start;

#endif
