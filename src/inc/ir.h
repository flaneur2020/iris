#ifndef IR_H
#define IR_H

#include "object.h"

typedef struct ir {
    unsigned int        s_gcstat;
    unsigned int        s_throttle;
    ir_object_t        *s_root_objects;
    ir_object_t         s_sweep_objects;
} ir_t;

typedef struct ir_state {
} ir_state_t;

ir_object_t*    ir_gc_malloc(ir_t *ir, size_t size);
int             ir_gc_link(ir_object_t *obj, ir_object_t *obj_child);
int             ir_gc_collect(ir_t *ir);

#endif
