#include "inc/iris.h"

int ir_heap_init(IrVM *vm);
int ir_heap_grow(IrVM *vm);

/* ------------------------------------------------- */

int ir_heap_init(IrVM *vm) {
    ir_heap_grow(vm);
    return 0;
}

int ir_heap_grow(IrVM *vm) {
    int nheaps;
    int heap_size;
    int nslots;
    void *mem;
    IrSlotBody *slot_bodies;
    IrSlot *slot;
    IrSlot *freeslots;
    int i;

    nheaps = vm->heaps_count;
    if (nheaps >= HEAPS_MAX)
        return -1;
    // 
    if (nheaps == 0)
        heap_size = HEAP_DEFAULT_SIZE;
    else
        heap_size = vm->heaps[nheaps-1].size * HEAP_INCREMENT_FACTOR;
    mem = malloc(heap_size);
    assert(mem != NULL);
    vm->heaps[nheaps].start = (unsigned long)mem;
    vm->heaps[nheaps].size = heap_size;
    vm->heaps_count++;
    ir_log("new heap: start: %lx\n", (unsigned long)mem);
    ir_log("new heap: end: %lx\n", (unsigned long)mem + heap_size);
    // resolve the heap into the fixed-size slots, and prepend them
    // all into the freeslots
    slot_bodies = (IrSlotBody*)mem;
    nslots = heap_size / sizeof(IrSlotBody);
    freeslots = &vm->freeslots;
    for (i=0; i<nslots; i++) {
        slot = (IrSlot*)&slot_bodies[i];
        slot->ohead.type = T_NONE;
        slot->next = freeslots->next;
        freeslots->next = slot;
    }
    return 0;
}

/* ------------------------------------------------- */

// allocate a object from the freeslots, if no available free slot 
// , it will trying gc, then grow the heap.
IrObject* ir_gc_newobj(IrVM *vm, int type){
    IrSlot *slot;
    IrObject *obj;
    
    if (vm->freeslots.next == NULL) {
        ir_log("no free memory now, trying gc & grow the heap\n");
        ir_gc_collect(vm);
        ir_heap_grow(vm);
    }
    slot = vm->freeslots.next;
    vm->freeslots.next = slot->next;
    // 
    memset(slot, 0, sizeof(IrSlot));
    obj = (IrObject*)slot;
    obj->type = type;
    ir_log("ir_gc_newobj: %lx\n", (VALUE)obj);
    return obj;
}

int ir_gc_freeobj(IrVM *vm, IrObject *obj){
    IrSlot *slot;
    
    ir_log("ir_gc_freeobj: %lx type: %x\n", (VALUE)obj, obj->type);
    slot = (IrSlot*)obj;
    slot->ohead.type = T_NONE;
    slot->next = vm->freeslots.next;
    vm->freeslots.next = slot;
    return 0;
}

// 
int is_pointer_to_heap(IrVM *vm, VALUE p){
    int i;
    IrHeap *heap;

    // check alignment
    if ((p & 3) != 0) {
        return 0;
    }
    // heap
    for (i=0; i<vm->heaps_count; i++) {
        heap = &vm->heaps[i];
        if ((p >= heap->start) && (p < heap->start + heap->size)) {
            return 1;
        }
    }
    return 0;
}

/* ------------------------------------------------- */

// mark an object recursively.
int ir_gc_mark(IrObject *obj) {
    if (obj->type == T_NONE)
        return 0;
    if (obj->flag & FL_MARK)  
        return 0;
    ir_log("ir_gc_mark: obj: %lx type: %x\n", (VALUE)obj, obj->type);
    obj->flag |= FL_MARK;
    return 0;
}

//
int ir_gc_mark_stack(IrVM *vm){
    unsigned long stack_top;
    VALUE *sp;
    VALUE val;
    jmp_buf jbuf;
    
    // save the current regisiters into stack
    setjmp(jbuf);
    // 
    stack_top = get_stack_pointer();
    ir_log("ir_gc_mark_stack: stack size: %lu bytes\n", ir_stack_start - stack_top);
    ir_log("ir_gc_mark_stack: stack range: %lx - %lx\n", stack_top, ir_stack_start);
    //
    for (sp=(VALUE*)stack_top; sp<(VALUE*)ir_stack_start; sp++) {
        val = *sp;
        if (! is_pointer_to_heap(vm, val))
            continue;
        ir_gc_mark((IrObject*)val);
    }
    return 0;
}

int ir_gc_sweep(IrVM *vm){
    int i;
    int j;
    int nslots;
    IrHeap *heap;
    IrSlot *slot;
    IrObject *obj;
    IrSlotBody *slot_bodies;

    // tranverse heaps
    for (i=0; i<vm->heaps_count; i++) {
        heap = &vm->heaps[i];
        slot_bodies = (IrSlotBody*)heap->start;
        nslots = heap->size / sizeof(IrSlotBody);
        for (j=0; j<nslots; j++) {
            slot = (IrSlot*)&slot_bodies[j];
            if (slot->ohead.type == T_NONE)
                continue;
            obj = (IrObject*)slot;
            if (obj->flag & FL_MARK)
                obj->flag &= ~FL_MARK;
            else
                ir_gc_freeobj(vm, obj);
        }
    }
    return 0;
}

int ir_gc_collect(IrVM *vm) {
    ir_gc_mark_stack(vm);
    ir_gc_sweep(vm);
    return 0;
}
