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
    TK_FUNCTION,
    TK_NEWLINE,
};

extern char* tkstr[];

#define TKSTR(tk) (tkstr[(tk)])

typedef struct IrToken {
    int token;
    char *buf;
    int buf_size;
} IrToken;

typedef struct IrLex {
    IrToken current;
    IrToken lookahead;
    char ch;
    int  line;
    int  col;
    FILE *file;
    char *path;
} IrLex;

#endif
