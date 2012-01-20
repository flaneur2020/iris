#include <iostream>
#include <cstdarg>
#include "iris.h"
#include "lexer.h"
#include "parser.h"

using namespace std;
using namespace iris;

void test_lexer() {
    const char *path = "test/sample.lua";
    FILE *fp = fopen(path, "r");
    Lexer lexer(fp, path);
    while(int t = lexer.next()){
        printf("- %s %s\n", tk2str(t), lexer.current()->buf.c_str());
    }
}

void test_parser() {
    const char *path = "test/sample.lua";
    FILE *fp = fopen(path, "r");
    Parser parser(fp, path);
    parser.parse();
}

int main(int argc, char* argv[]){
    test_parser();

    return 0;
}

void iris::iris_debug(const char *fmt, ...) {
#ifdef NDEBUG
    va_list vp;
    va_start(vp, fmt); 
    vfprintf(stderr, fmt, vp);
    va_end(vp);
#endif
}
