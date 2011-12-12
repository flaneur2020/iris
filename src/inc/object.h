#ifndef OBJECT_H
#define OBJECT_H

enum {
    TInternal = 0,
    TInteger = 1,
    TTable,
    TString,
    TClosure,
    TObject
};

typedef struct ir_object {
    unsigned int type;
    unsigned int flag;
    struct IrObject *gc_next;
} ir_object_t;

#endif
