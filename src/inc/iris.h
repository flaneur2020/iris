#ifndef IRIS_H
#define IRIS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include "st.h"
#include "vector.h"

#include "vm.h"
#include "lex.h"
#include "parser.h"
#include "object.h"


#define ir_log printf

static inline unsigned long get_stack_pointer(){
    unsigned long p;
    asm("mov %%esp, %%eax":"=a"(p));
    return p;
}

int ir_vm_init(IrVM *vm);
int ir_vm_close(IrVM *vm);

int ir_heap_init(IrVM *vm);
int ir_heap_grow(IrVM *vm);

IrObject* ir_gc_newobj(IrVM *vm, int type);
int ir_gc_freeobj(IrVM *vm, IrObject *obj);
int ir_gc_mark(IrObject *obj);
int ir_gc_mark_stack(IrVM *vm);
int ir_gc_sweep(IrVM *vm);
int ir_gc_collect(IrVM *vm);
int is_pointer_to_heap(IrVM *vm, VALUE p);

int ir_parse(IrVM *vm, char *path);

int ir_lex_init(IrLex *lp, char *path);
int ir_lex_next(IrLex *lp);
int ir_lex_close(IrLex *lp);
int ir_lex_error(IrLex *lp);

extern char *tkstr[];

#endif
