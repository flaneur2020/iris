#ifndef ILEX_H
#define ILEX_H

#define NTOKEN_SIZE 4096

enum {
    TK_NUMBER = 1, 
    TK_STRING,
    TK_NAME,
    TK_AND,
    TK_OR,
    TK_IF,
    TK_LOCAL,
    TK_NIL,
    TK_FOR,
    TK_WHILE,
    TK_BREAK,
    TK_CONTINUE,
};

typedef struct irlex {
    char l_buf[NTOKEN_SIZE];
    char l_buf_size;
    int  l_line;
    int  l_col;
    FILE *l_file;
} irlex_t;

int irX_init(irlex_t *lp, char *path);
int irX_next(irlex_t *lp);
int irX_close(irlex_t *lp);
int irX_error(irlex_t *lp);

int irX_consume(irlex_t *lp);
int irX_getc(irlex_t *lp);
int irX_ungetc(irlex_t *lp);

#endif
