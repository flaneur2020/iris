#ifndef IRIS_H
#define IRIS_H

#include <cstdio>

#define NDEBUG 1

namespace iris {
    class VM;

    void iris_debug(const char *fmt, ...);
}

#endif
