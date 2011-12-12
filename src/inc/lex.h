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

typedef struct IrLex {
    char l_current;
    char l_buf[NTOKEN_SIZE];
    char l_buf_size;
    int  l_line;
    int  l_col;
    FILE *l_file;
    char *l_path;
} IrLex;

int ir_lex_init(IrLex *lp, char *path);
int ir_lex_next(IrLex *lp);
int ir_lex_close(IrLex *lp);
int ir_lex_error(IrLex *lp);

int ir_lex_consume(IrLex *lp, char c);
int ir_lex_getc(IrLex *lp);
int ir_lex_ungetc(IrLex *lp);

char ir_lex_number(IrLex *lp);
char ir_lex_string(IrLex *lp, char qc);
char ir_lex_spaces(IrLex *lp);
char ir_lex_digits(IrLex *lp);

#endif
