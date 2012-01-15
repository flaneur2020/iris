#include "iris.h"

#define ir_parse_error(pp, fmt, ...) \
    do { fprintf(stderr, "Parse Error: %s:%d:%d: " fmt "\n", (pp)->lex.path, (pp)->lex.line, (pp)->lex.col, ##__VA_ARGS__); exit(1); } while (0)

static int ir_parse_chunk(IrParser *pp);
static int ir_parse_stat(IrParser *pp);
static int ir_parse_laststat(IrParser *pp);

int ir_parser_init(IrParser *pp, IrVM *vm, char *path){
    ir_lex_init(&pp->lex, path);
    pp->vm = vm;
    return 0;
}

int ir_parser_close(IrParser *pp){
    ir_lex_close(&pp->lex);
    return 0;
}

int ir_parse(IrVM *vm, char *path){
    IrParser parser;

    ir_parser_init(&parser, vm, path);
    ir_parse_chunk(&parser);
    return 0;
}

/* ----------------------------------------- */

/*
 * chunk := (stat (';')?)* (laststat (';')?)?
 * */
static int ir_parse_chunk(IrParser *pp){
    int tk;

    while(ir_parse_stat(pp) != 0) {
        tk = ir_lex_next(&pp->lex);
        if (tk != TK_NEWLINE || tk != ';') 
            ir_parse_error(pp, "expected ; or \\n");
    }
    if (ir_parse_laststat(pp) != 0) {
        tk = ir_lex_next(&pp->lex);
        if (tk != TK_NEWLINE || tk != ';') 
            ir_parse_error(pp, "expected ; or \\n");
    }
    return 0;
}

static int ir_parse_stat(IrParser *pp){
    int tk;

    tk = ir_lex_next(&pp->lex);
    return 0;
}

static int ir_parse_laststat(IrParser *pp){
    return 0;
}
