#include <iostream>
#include "iris.h"
#include "lexer.h"
#include "parser.h"

using namespace std;
using namespace iris;

int main(int argc, char* argv[]){
    const char *path = "test/sample.lua";
    FILE *fp = fopen(path, "r");
    Parser parser(fp, path);
    
    parser.parse();
    return 0;
}
