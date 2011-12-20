#include "inc/iris.h"

#define ir_parse_error(pp, fmt, ...) \
    do { fprintf(stderr, "Parse Error: %s:%d:%d: " fmt "\n", (pp)->lex.path, (pp)->lex.line, (pp)->lex.col, ##__VA_ARGS__); exit(1); } while (0)

static int ir_parse_chunk(IrParser *pp);
static int ir_parse_stat(IrParser *pp);
static int ir_parse_laststat(IrParser *pp);

int ir_parser_init(IrParser *pp, char *path){
    return 0;
}

int ir_parser_close(){
    return 0;
}

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
    ir_lex_init(&lex, path);
    ir_parse_chunk(&parser);
    //  
    fclose(fp);
    return 0;
}

/* ----------------------------------------- */

/*
 * chunk := (stat (';')?)* (laststat (';')?)?
 * */
static int ir_parse_chunk(IrParser *pp){
    int tk;

    while(ir_parse_stat(pp) == 0) {
        tk = ir_lex_next(&pp->lex);
        if (tk != TK_NEWLINE) 
            ir_parse_error(pp, "expected ; or \\n");
    }
    if (ir_parse_laststat(pp) == 0) {
        if (tk != TK_NEWLINE || tk != ';') 
            ir_parse_error(pp, "expected ; or \\n");
    }
    return 0;
}

static int ir_parse_stat(IrParser *pp){
    return 0;
}

static int ir_parse_laststat(IrParser *pp){
    return 0;
}
