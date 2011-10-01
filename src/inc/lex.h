#ifndef ILEX_H
#define ILEX_H

#define NTOKEN_SIZE 4096

enum {
    // terminal symbols
    TK_NUMBER = 257,
    TK_STRING,
    TK_NAME,
    // operators
    TK_LTE,
    TK_GTE,
    TK_EQ,
    // keywords
    TK_AND,
    TK_OR,
    TK_IF,
    TK_LOCAL,
    TK_NIL,
    TK_FOR,
    TK_WHILE,
    TK_BREAK,
    TK_CONTINUE,
    TK_NEWLINE,
};

extern char* tkstr[];

#define TKSTR(tk) (tkstr[(tk)])

typedef struct irlex {
    char l_current;
    char l_buf[NTOKEN_SIZE];
    char l_buf_size;
    int  l_line;
    int  l_col;
    FILE *l_file;
    char *l_path;
} irlex_t;

int irX_init(irlex_t *lp, char *path);
int irX_next(irlex_t *lp);
int irX_close(irlex_t *lp);
int irX_error(irlex_t *lp);

int irX_consume(irlex_t *lp, char c);
int irX_getc(irlex_t *lp);
int irX_ungetc(irlex_t *lp);

char irT_number(irlex_t *lp);
char irT_string(irlex_t *lp, char qc);
char irT_spaces(irlex_t *lp);
char irT_digits(irlex_t *lp);

#endif
