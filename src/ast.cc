#include "ast.h"

using namespace iris;

AstNode* AstPool::new_node(int type) {
    AstNode *n = new AstNode();
    _nodes.push_back(n);
    return n;
}

AstPool::~AstPool() {
    free_nodes();
}

/* Free all the ast nodes.
 * */
void AstPool::free_nodes() {
    AstNode *n;
    deque<AstNode*>::iterator it;

    for (it = _nodes.begin(); it != _nodes.end(); ++it) {
        n = *it;
        delete n;
    }
    _nodes.clear();
}


