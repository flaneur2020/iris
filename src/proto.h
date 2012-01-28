#ifndef PROTO_H
#define PROTO_H

#include <vector>

namespace iris {

typedef unsigned long Val;

class Proto {
    vector<char> opcodes;
    vector<Val> locals;
    vector<Val> upvals;
};

}

#endif
