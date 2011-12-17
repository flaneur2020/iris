#include "inc/iris.h"

int ir_parse_root(IrVM *vm, IrParser *pp, IrLex *lp);

int ir_parse(IrVM *vm, char *path){
    IrLex lex; 
    IrParser parser;
    FILE *fp;

    // open file
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "File Not Found: %s\n", path);
        exit(1);
    }
    // init lexer
    ir_lex_init(&lex, fp, path);
    ir_parse_root(vm, &parser, &lex);
    //  
    fclose(fp);
    return 0;
}

/* ----------------------------------------- */

int ir_parse_root(IrVM *vm, IrParser *pp, IrLex *lp){
    return 0;
}
