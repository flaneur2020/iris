#ifndef AST_H
#define AST_H

#include <deque>
#include <vector>
#include "iris.h"

using namespace std;

namespace iris {

struct AstNode {
    int type;
    int val;
    vector<Obj> nodes;

public:
    AstNode() 
        : type(0)
        , val(0)
        , nodes(2, 0) { };
};

/* All the AstNode objects are allocated from the AstPool object, 
 * and deallocated at the same time.
 * */
class AstPool {
    deque<AstNode*> _nodes;

public:
    AstPool()
        : _nodes() {};

    AstNode* new_node(int type);
    void free_nodes();
};

}

#endif
