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

typedef struct IrObject {
    unsigned int o_type;
    unsigned int o_flag;
    struct IrObject *o_gc_next;
} IrObject;

#endif
