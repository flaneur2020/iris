#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

namespace iris {

struct Token {
    int token;
    string buf;

    explicit Token()
        : token(0)
        , buf("") {}
};

/*
 * for a token > TK_KW, it is a keyword
 * */
enum {
    TK_EOI = 0,
    // terminal symbols
    TK_NUMBER = 257,
    TK_STRING,
    TK_NAME,
    // operators
    TK_LTE,
    TK_GTE,
    TK_EQ,
    // keywords
    TK_KW, // as a delimeter
    TK_AND,
    TK_OR,
    TK_IF,
    TK_LOCAL,
    TK_NIL,
    TK_FOR,
    TK_DO,
    TK_REPEAT,
    TK_WHILE,
    TK_BREAK,
    TK_CONTINUE,
    TK_FUNCTION,
    TK_RETURN,
    TK_NEWLINE
};

static int tkkeywords[] = { 
    TK_AND, TK_OR, TK_IF, TK_LOCAL, TK_NIL, 
    TK_FOR, TK_WHILE, TK_BREAK, TK_CONTINUE, TK_FUNCTION 
};

const int NTKKEYWORDS = sizeof(tkkeywords) / sizeof(int);

const char* tk2str(int t);

}

#endif
