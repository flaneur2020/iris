#include "inc/iris.h"

int irX_init(irlex_t *lp, char *path) {
    FILE *fp;

    // open file
    fp = fopen(path, "r");
    if (fp == NULL) {
        irX_error(lp, "File Not Found.");
        return -1;
    }
    // misc init 
    lp->l_file = fp;
    lp->l_line = 0;
    lp->l_col  = 0;
}

int irX_close(irlex_t *lp) {
    fclose(lp->l_file);
}

int irX_error(irlex_t *lp, char *str) {
    fprintf(stderr, "Lex Error %s:%d:%d: %s\n", lp->l_file, lp->l_line, lp->l_col, str);
    exit(1);
    return -1;
}

/* --------------------------------------------- */

int irX_next(irlex_t *lp) {
    char c;
    int r;

    irX_reset_buf(lp);
    while(1) {
        c = irX_step(lp);
        switch(c){
        case '\n':
        case '\r':
            break;
        default:
            if (isdigit(c)) {
                irX_number(c);
                return TK_NUMBER;
            }
        }
    }
}

int irX_step(irlex_t *lp){
    char c;

    c = irX_getc(lp);
    if (c == '\n') {
        lp->l_line++;
        lp->l_col = 0;
    }
    else {
        lp->l_col++;
    }
    return c;
}

int irX_consume(irlex_t *lp, char c) {
    lp->l_buf[lp->l_buf_size++] = c;
}

int irX_reset_buf(irlex_t lp){
    lp->l_buf_size = 0;
}

/* -------------------------------------------- */

int irX_getc(irlex_t *lp) {
    return fgetc(lp->l_file);
}

/* -------------------------------------------- */

int irX_number(irlex_t *lp, char c){
    
}
