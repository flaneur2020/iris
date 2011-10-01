#include "inc/iris.h"

#define irX_error(lp, fmt, ...) \
    do { fprintf(stderr, "Lex Error: %s:%d:%d" fmt, lp->l_path, lp->l_line, lp->l_col, ##__VA_ARGS__); exit(1); } while (0)

int irX_init(irlex_t *lp, char *path) {
    FILE *fp;

    // open file
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "File Not Found: %s\n", path);
        exit(1);
    }
    // misc init 
    lp->l_path = path;
    lp->l_file = fp;
    lp->l_line = 0;
    lp->l_col  = 0;
    lp->l_current = '\0';
    // read the first char
    irX_step(lp);
}

int irX_close(irlex_t *lp) {
    fclose(lp->l_file);
}

/* --------------------------------------------- */

// Fetch one token each time, returns the type of token,
// and store the content of this token into lp->l_buf[].
// On fetching finished, returns 0.
int irX_next(irlex_t *lp) {
    char c;
    int r;

    irX_reset_buf(lp);
    while((c = lp->l_current) != EOF && c != '\0') {
        switch(c){
        case '\n':
        case '\r':
            return TK_NEWLINE;
        case '\'':
        case '\"':
            irT_string(lp, c);
            return TK_STRING;
        default:
            if (isspace(c)) {
                irT_spaces(lp);
                continue;
            }
            if (isdigit(c)) {
                irT_number(lp);
                return TK_NUMBER;
            }
        }
    }
    return 0;
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
    return (lp->l_current = c);
}

int irX_consume(irlex_t *lp, char c) {
    lp->l_buf[lp->l_buf_size++] = c;
}

int irX_reset_buf(irlex_t *lp){
    lp->l_buf_size = 0;
}

/* -------------------------------------------- */

int irX_getc(irlex_t *lp) {
    return fgetc(lp->l_file);
}

/* -------------------------------------------- */

// digits { . digits }?
char irT_number(irlex_t *lp){
    char c;

    irT_digits(lp);
    // if it's an decimal
    if ((c = lp->l_current) == '.') {
        irX_consume(lp, c);
        irX_digits(lp);
    }
    return c;
}

// \d+
char irT_digits(irlex_t *lp) {
    char c = lp->l_current;
    
    if (!isdigit(c)) 
        irX_error(lp, "number expected, but got: %c", c);
    while (isdigit(c)) {
        irX_consume(lp, c);
        c = irX_step(lp);
    }
    return c;
}

// ' '
char irT_string(irlex_t *lp, char qc) {
    char c = lp->l_current;

    while ((c = irX_step(lp)) != qc) {
        switch(c) {
        case EOF:
        case '\0':
        case '\n':
        case '\r':
            irX_error("unfinished string");
        case '\\':
            switch(c = irX_step(lp)) {
            case 'n': irX_consume(lp, '\n'); break;
            case 'r': irX_consume(lp, '\r'); break;
            case 'a': irX_consume(lp, '\a'); break;
            case 'b': irX_consume(lp, '\b'); break;
            case 'f': irX_consume(lp, '\f'); break;
            case 'v': irX_consume(lp, '\v'); break;
            case '\\': irX_consume(lp, '\\'); break;
            case '\"': irX_consume(lp, '\"'); break;
            case '\'': irX_consume(lp, '\''); break;
            case '\?': irX_consume(lp, '\?'); break;
            // go through
            case '\n':
            case '\r': irX_consume(lp, '\n'); break;
            default: 
            }
        }
    }
    return c;
}

// \s*
char irT_spaces(irlex_t *lp) {
    char c = lp->l_current;

    if (!isspace(c)) 
        return TK_SPACES;
    while (isspace(c)) {
        c = irX_step(lp);
    }
    return c:
}

/* ----------------------------- */
