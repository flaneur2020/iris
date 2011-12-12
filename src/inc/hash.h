#ifndef HASH_H
#define HASH_H

#include "object.h"

#define BUCKETS_DEFAULT_SIZE 256

typedef struct ir_bucket {
} ir_bucket_t;

typedef struct ir_hash {
    ir_object_t *o_header;
    unsigned int h_buckets_size;
    ir_bucket_t *h_buckets;
} ir_hash_t;

#endif
