#ifndef IR_H
#define IR_H

#include "object.h"

typedef struct IrVM {
    unsigned int        v_gcstat;
    unsigned int        v_throttle;
    IrObject           *v_root_objects;
    IrObject           *v_sweep_objects;
} IrVM;

typedef struct IrState {
} IrState;

IrObject*    ir_gc_malloc(IrVM *ir, size_t size);
int          ir_gc_link(IrObject *obj, IrObject *obj_child);
int          ir_gc_collect(IrVM *vm);

#endif
