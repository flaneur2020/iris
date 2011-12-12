#include "inc/iris.h"

/* ------------------------------------------------- */

/* 
 * Allocate 
 * */
ir_object_t* ir_gc_malloc(ir_t *ir, size_t size) {
    ir_object_t *obj;
    
    obj = (ir_object_t*)malloc(size);
}

int ir_gc_free(ir_object_t *obj){
}

int ir_gc_link(ir_object_t *obj, ir_object_t *obj_child) {
}

int ir_gc_collect(ir_t *ir) {
}
