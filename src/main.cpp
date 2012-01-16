#include <iostream>
#include "iris.h"
#include "lexer.h"

using namespace std;

int main(int argc, char* argv[]){
    const char *path = "test/sample.lua";
    FILE *fp = fopen(path, "r");
    Lexer lexer(fp, path);
    int tk;
    
    while ((tk = lexer.next())) {
        cout << tk2str(tk) << endl;
    }
    return 0;
}
