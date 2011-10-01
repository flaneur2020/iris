#include "inc/iris.h"

char *tkstr[] = {
    ['.'] = ".",
    ['+'] = "+",
    ['-'] = "-",
    ['*'] = "*",
    ['/'] = "/",
    [':'] = ":",
    [','] = ",",
    ['>'] = ">",
    ['<'] = "<",
    [TK_GTE] = "GTE",
    [TK_LTE] = "LTE",
    [TK_NUMBER] = "<number>",
    [TK_STRING] = "<string>",
    [TK_NAME]  = "NAME",
    [TK_AND] = "AND",
    [TK_OR] = "OR",
    [TK_IF] = "IF", 
    [TK_NIL] = "NIL",
    [TK_FOR] = "FOR",
    [TK_WHILE] = "WHILE",
    [TK_BREAK] = "BREAK",
    [TK_CONTINUE] = "CONTINUE",
    [TK_NEWLINE] = "NEWLINE",
    [TK_LOCAL] = "LOCAL"
};


#define irX_error(lp, fmt, ...) \
    do { fprintf(stderr, "Lex Error: %s:%d:%d: " fmt "\n", (lp)->l_path, (lp)->l_line, (lp)->l_col, ##__VA_ARGS__); exit(1); } while (0)

/* ------------------------------------------ */

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

    while((c = lp->l_current) != EOF && c != '\0') {
        irX_reset_buf(lp, 0);
        if (c == '<') {
            c = irX_step(lp);
            if (c == '=')  
                return TK_LTE;
            return '<';
        }
        if (c == '>') {
            c = irX_step(lp);
            if (c == '=')  
                return TK_GTE;
            return '>';
        }
        if (c == '-') {
            c = irX_step(lp);
            if (c != '-')
                return '-';
            // else it's a comment
            irX_step_until(lp, "\n\r");
            continue;
        }
        // assign or eual
        if (c == '=') {
            c = irX_step(lp);
            if (c != '=')
                return '=';
            irX_step(lp);
            return TK_EQ;
        }
        // newline
        if (strchr("\n\r", c)) {
            irX_step(lp);
            return TK_NEWLINE;
        }
        // skip the whitespaces
        if (isspace(c)) {
            irT_spaces(lp);
            continue;
        }
        // single character operators
        if (strchr(",.+:*/", c)) {
            irX_step(lp);
            return c;
        }
        // string
        if (strchr("\"\'", c)) {
            irT_string(lp, c);
            return TK_STRING;
        }
        //
        if (isdigit(c)) {
            irT_number(lp);
            return TK_NUMBER;
        }
        irX_error(lp, "unkown token: %c", c);
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

int irX_step_until(irlex_t *lp, char *str){
    while (!strchr(str, irX_step(lp)));
}

int irX_consume(irlex_t *lp, char c) {
    lp->l_buf[lp->l_buf_size++] = c;
}

int irX_reset_buf(irlex_t *lp, int size){
    lp->l_buf_size = 0;
}

/* -------------------------------------------- */

int irX_getc(irlex_t *lp) {
    return fgetc(lp->l_file);
}

/* -------------------------------------------- */

// digits { . digits }?
char irT_number(irlex_t *lp){
    char c = lp->l_current;

    irT_digits(lp);
    // if it's an decimal
    if ((c = lp->l_current) == '.') {
        irX_consume(lp, c);
        irT_digits(lp);
    }
    irX_consume(lp, '\0');
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
            irX_error(lp, "unfinished string");
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
            }
        default:
            irX_consume(lp, c);
        }
    }
    irX_step(lp);
    return c;
}

// \s*
char irT_spaces(irlex_t *lp) {
    char c = lp->l_current;

    if (!isspace(c)) 
        return c;
    while (isspace(c)) {
        c = irX_step(lp);
    }
    return c;
}

/* ----------------------------- */
