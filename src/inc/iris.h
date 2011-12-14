#ifndef IRIS_H
#define IRIS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vm.h"
#include "lex.h"
#include "object.h"

#define ir_log printf

static inline unsigned long get_stack_pointer(){
    unsigned long p;
    asm("mov %%esp, %%eax":"=a"(p));
    return p;
}

#endif
