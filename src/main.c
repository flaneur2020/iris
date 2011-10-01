#include "inc/iris.h"

int main(int argc, char **argv) {
    irlex_t lex;
    int tk;
    char *path = "test/sample.lua";

    irX_init(&lex, path);
    while ((tk = irX_next(&lex)) > 0) {
        if (TKSTR(tk)[0] == '<') 
            printf("%s %s\n", TKSTR(tk), lex.l_buf);
        else 
            printf("%s \n", TKSTR(tk));
    }
    
    return 0;
}
