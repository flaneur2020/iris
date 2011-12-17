#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define VECTOR_GROWTH_FACTOR 2
#define VECTOR_DEFAULT_MAX_COUNT 8

struct ir_vector {
    size_t entry_size;
    size_t count;
    size_t max_count;
    void  *entries;
};

struct ir_vector* iv_new(size_t entry_size, size_t max_count);
int iv_grow(struct ir_vector *iv);
int iv_free(struct ir_vector *iv);

int iv_insert(struct ir_vector *iv, unsigned int i, void *val);
void* iv_find(struct ir_vector *iv, unsigned int i);
void* iv_append(struct ir_vector *iv);

#define IV_FIND(iv, i) \
    (void*)((unsigned long)(iv)->entries + (i) * (iv)->entry_size);

#endif

