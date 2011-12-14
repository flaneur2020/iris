#ifndef GC_H
#define GC_H

#include "object.h"

typedef union IrSlot {
    union IrSlot *next;
    union {
        struct IrArray array;
        struct IrString string;
    } data;
} IrSlot;

typedef struct IrHeap {
    unsigned long start;
    size_t size;
} IrHeap;

#define HEAPS_MAX 16
#define HEAP_DEFAULT_SIZE 4096
#define HEAP_INCREMENT_FACTOR 4

unsigned long ir_stack_start;

#endif
