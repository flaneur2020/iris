#ifndef PROTO_H
#define PROTO_H

#include "st.h"
#include "vector.h"

typedef struct IrProto {
    struct ir_vector *opcodes;
} IrProto;

#endif
