#ifndef LEXER_H
#define LEXER_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include "token.h"

using namespace std;

namespace iris {

class Lexer {
    FILE *_file;
    const char *_file_name;
    Token _current;
    Token _ahead;
    int _line;
    int _col;
    char _ch;

public: 
    explicit Lexer(FILE *file, const char *file_name);
    ~Lexer();

    int next();
    const Token* current() const;
    const Token* lookahead() const;
    void lex_error(const char *fmt, ...);

private:
    char next_char();
    char step();
    char step_until(const char *str);
    char consume(char c);
    void reset_buf();
    int lex();

    char tname();
    char tnumber();
    char tstring(char qc);
    char tspaces();
    char tdigits();
};

struct lex_exception : std::exception {};

}

#endif
