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


#define ir_lex_error(lp, fmt, ...) \
    do { fprintf(stderr, "Lex Error: %s:%d:%d: " fmt "\n", (lp)->path, (lp)->line, (lp)->col, ##__VA_ARGS__); exit(1); } while (0)

/* ------------------------------------------ */

int ir_lex_init(IrLex *lp, char *path) {
    FILE *fp;

    // open file
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "File Not Found: %s\n", path);
        exit(1);
    }
    // misc init 
    lp->path = path;
    lp->file = fp;
    lp->line = 0;
    lp->col  = 0;
    lp->current = '\0';
    // read the first char
    ir_lex_step(lp);
    return 0;
}

int ir_lex_close(IrLex *lp) {
    fclose(lp->file);
    return 0;
}

/* --------------------------------------------- */

// Fetch one token each time, returns the type of token,
// and store the content of this token into lp->buf[].
// On fetching finished, returns 0.
int ir_lex_next(IrLex *lp) {
    char c;

    while((c = lp->current) != EOF && c != '\0') {
        ir_lex_reset_buf(lp, 0);
        if (c == '<') {
            c = ir_lex_step(lp);
            if (c == '=')  
                return TK_LTE;
            return '<';
        }
        if (c == '>') {
            c = ir_lex_step(lp);
            if (c == '=')  
                return TK_GTE;
            return '>';
        }
        if (c == '-') {
            c = ir_lex_step(lp);
            if (c != '-')
                return '-';
            // else it's a comment
            ir_lex_step_until(lp, "\n\r");
            continue;
        }
        // assign or eual
        if (c == '=') {
            c = ir_lex_step(lp);
            if (c != '=')
                return '=';
            ir_lex_step(lp);
            return TK_EQ;
        }
        // newline
        if (strchr("\n\r", c)) {
            ir_lex_step(lp);
            return TK_NEWLINE;
        }
        // skip the whitespaces
        if (isspace(c)) {
            ir_lex_spaces(lp);
            continue;
        }
        // single character operators
        if (strchr(",.+:*/", c)) {
            ir_lex_step(lp);
            return c;
        }
        // string
        if (strchr("\"\'", c)) {
            ir_lex_string(lp, c);
            return TK_STRING;
        }
        //
        if (isdigit(c)) {
            ir_lex_number(lp);
            return TK_NUMBER;
        }
        ir_lex_error(lp, "unkown token: %c", c);
    }
    return 0;
}

int ir_lex_step(IrLex *lp){
    char c;

    c = ir_lex_getc(lp);
    if (c == '\n') {
        lp->line++;
        lp->col = 0;
    }
    else {
        lp->col++;
    }
    return (lp->current = c);
}

int ir_lex_step_until(IrLex *lp, char *str){
    while (!strchr(str, ir_lex_step(lp)));
    return 0;
}

int ir_lex_consume(IrLex *lp, char c) {
    lp->buf[lp->buf_size++] = c;
    return 0;
}

int ir_lex_reset_buf(IrLex *lp, int size){
    lp->buf_size = 0;
    return 0;
}

/* -------------------------------------------- */

int ir_lex_getc(IrLex *lp) {
    return fgetc(lp->file);
}

/* -------------------------------------------- */

// digits { . digits }?
char ir_lex_number(IrLex *lp){
    char c = lp->current;

    ir_lex_digits(lp);
    // if it's an decimal
    if ((c = lp->current) == '.') {
        ir_lex_consume(lp, c);
        ir_lex_digits(lp);
    }
    ir_lex_consume(lp, '\0');
    return c;
}

// \d+
char ir_lex_digits(IrLex *lp) {
    char c = lp->current;
    
    if (!isdigit(c)) 
        ir_lex_error(lp, "number expected, but got: %c", c);
    while (isdigit(c)) {
        ir_lex_consume(lp, c);
        c = ir_lex_step(lp);
    }
    return c;
}

// ' '
char ir_lex_string(IrLex *lp, char qc) {
    char c = lp->current;

    while ((c = ir_lex_step(lp)) != qc) {
        switch(c) {
        case EOF:
        case '\0':
        case '\n':
        case '\r':
            ir_lex_error(lp, "unfinished string");
        case '\\':
            switch(c = ir_lex_step(lp)) {
            case 'n': ir_lex_consume(lp, '\n'); break;
            case 'r': ir_lex_consume(lp, '\r'); break;
            case 'a': ir_lex_consume(lp, '\a'); break;
            case 'b': ir_lex_consume(lp, '\b'); break;
            case 'f': ir_lex_consume(lp, '\f'); break;
            case 'v': ir_lex_consume(lp, '\v'); break;
            case '\\': ir_lex_consume(lp, '\\'); break;
            case '\"': ir_lex_consume(lp, '\"'); break;
            case '\'': ir_lex_consume(lp, '\''); break;
            case '\?': ir_lex_consume(lp, '\?'); break;
            // go through
            case '\n':
            case '\r': ir_lex_consume(lp, '\n'); break;
            }
        default:
            ir_lex_consume(lp, c);
        }
    }
    ir_lex_step(lp);
    return c;
}

// \s*
char ir_lex_spaces(IrLex *lp) {
    char c = lp->current;

    if (!isspace(c)) 
        return c;
    while (isspace(c)) {
        c = ir_lex_step(lp);
    }
    return c;
}

/* ----------------------------- */
