#ifndef OBJECT_H
#define OBJECT_H

typedef unsigned long VALUE;

typedef struct IrObject {
    unsigned int type;
    unsigned int flag;
} IrObject;

typedef struct IrArray {
    struct ir_object *ohead;
    VALUE *values;
    size_t size;
} IrArray;

typedef struct IrString {
    struct ir_object *ohead;
    char *cstring;
    size_t size;
} IrString;

#endif
