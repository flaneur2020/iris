#ifndef IRIS_H
#define IRIS_H

#include <cstdio>

typedef unsigned long Obj;
typedef unsigned long Addr;

/* As tagged pointer */
#define OFLAG_NUM 0x1

/* Internal constants */
#define Vnil 0
#define Vfalse 1
#define Vtrue 1

/* Utils */
void iris_debug(const char *fmt, ...);

#define NDEBUG 1
#endif
