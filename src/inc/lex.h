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

typedef struct ir_lex {
    char l_current;
    char l_buf[NTOKEN_SIZE];
    char l_buf_size;
    int  l_line;
    int  l_col;
    FILE *l_file;
    char *l_path;
} ir_lex_t;

int ir_lex_init(ir_lex_t *lp, char *path);
int ir_lex_next(ir_lex_t *lp);
int ir_lex_close(ir_lex_t *lp);
int ir_lex_error(ir_lex_t *lp);

int ir_lex_consume(ir_lex_t *lp, char c);
int ir_lex_getc(ir_lex_t *lp);
int ir_lex_ungetc(ir_lex_t *lp);

char ir_lex_number(ir_lex_t *lp);
char ir_lex_string(ir_lex_t *lp, char qc);
char ir_lex_spaces(ir_lex_t *lp);
char ir_lex_digits(ir_lex_t *lp);

#endif
