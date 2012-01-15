#include <assert.h>
#include "vector.h"

struct ir_vector* iv_new(size_t entry_size, size_t default_max_count) {
    struct ir_vector *iv;

    assert(entry_size > 0);
    iv = (struct ir_vector*)malloc(sizeof(struct ir_vector));
    iv->count = 0;
    iv->entry_size = entry_size;
    iv->max_count = (default_max_count > VECTOR_DEFAULT_MAX_COUNT)? default_max_count: VECTOR_DEFAULT_MAX_COUNT;
    while(iv->max_count <= default_max_count)
        iv->max_count *= VECTOR_GROWTH_FACTOR;
    iv->entries = malloc(iv->max_count * iv->entry_size);
    return iv;
}

int iv_grow(struct ir_vector *iv) {
    assert(iv->max_count > 0);
    assert(iv->entry_size > 0);
    iv->max_count *= VECTOR_GROWTH_FACTOR;
    iv->entries = realloc(iv->entries, iv->max_count * iv->entry_size);
    return 0;
}

int iv_free(struct ir_vector *iv) {
    assert(iv != NULL);
    free(iv->entries);
    free(iv);
    return 0;
}

/* ---------------------------------------------- */

int iv_reset(struct ir_vector *iv){
    iv->count = 0;
    return 0;
}

int iv_insert(struct ir_vector *iv, unsigned int i, void* valp) {
    void *p;

    assert(valp != NULL);
    while (iv->max_count <= i)
        iv_grow(iv);
    if (iv->count <= i)
        iv->count = i+1;
    p = IV_FIND(iv, i);
    memcpy(p, valp, iv->entry_size);
    return 0;
}

void* iv_find(struct ir_vector *iv, unsigned int i) {
    if (i >= iv->count)
        return NULL;
    return IV_FIND(iv, i);
}

void* iv_append(struct ir_vector *iv, void *valp) {
    int i;
    void *p;

    iv->count++;
    if (iv->count >= iv->max_count) {
        iv_grow(iv);
    }
    i = iv->count-1;
    p = IV_FIND(iv, i);
    if (valp != NULL) {
        memcpy(p, valp, iv->entry_size);
    }
    return p;
}

/*------------------------------------------------- */

char* iv_append_char(struct ir_vector *iv, char c){
    assert(iv->entry_size == sizeof(char));
    return iv_append(iv, &c);
}
