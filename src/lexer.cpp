#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <map>
#include "lexer.h"
#include "token.h"

using namespace std;
using namespace iris;

/* ----------------------------------------------------- */

static map<string, int> *kwtab = NULL;

Lexer::Lexer(FILE *file, const char *file_name)
    : _file(file)
    , _file_name(file_name)
    , _current()
    , _ahead()
    , _line(1)
    , _col(0)
    , _ch('\0')
{
    // initialize the kwtab
    if (kwtab == NULL) {
        kwtab = new map<string, int>();
        for (int i = 0; i < NTKKEYWORDS; i++) {
            int     tk = tkkeywords[i];
            string str = tk2str(tk);
            kwtab->insert(make_pair(str, tk));
        }
    }
    // read the first char
    step();
    // read the first token
    next();
}

Lexer::~Lexer() {
}

/* ----------------------------------------------------- */

/*
 * Move the ahead token to current, and get the next token
 * to ahead.
 * Returns 0 on reached EOF, or an lex error raised(TODO).
 * */
int Lexer::next() {
    _current.token = _ahead.token;
    _current.buf   = _ahead.buf;
    try {
        _ahead.token = lex();
    }
    catch (lex_exception& e) {
        _ahead.token = TK_EOI;
    }
    return _current.token;
}

const Token* Lexer::current() const {
    return &_current;
}

const Token* Lexer::lookahead() const {
    return &_ahead;
}

int Lexer::line() const {
    return _line;
}

int Lexer::col() const {
    return _col;
}

/* ------------------------------------------- */

void Lexer::lex_error(const char *fmt, ...){
    va_list vp;
    va_start(vp, fmt); 
    fprintf(stderr, "Lex error: %s:%d:%d ", _file_name, _line, _col);
    vfprintf(stderr, fmt, vp);
    fprintf(stderr, "\n");
    va_end(vp);
    throw lex_exception();
}

/* ----------------------------------------------------- */

char Lexer::step() {
    char c;

    c = next_char();
    if (c == '\n') {
        _line++;
        _col = 0;
    }
    else {
        _col++;
    }
    return (_ch = c);
}

char Lexer::step_until(const char *str) {
    while (! strchr(str, step()));
    return '\0';
}

char Lexer::consume(char c){
    _ahead.buf.push_back(c);
    return c;
}

void Lexer::reset_buf(){
    _ahead.buf.clear();
}

char Lexer::next_char() {
    return fgetc(_file);
}

int Lexer::lex() {
    char c;
    int r;
    int tk;
    map<string, int>::iterator ri;

    while((c = _ch) != EOF && c != '\0') {
        reset_buf();
        if (c == '<') {
            c = step();
            if (c == '=')  
                return TK_LTE;
            return '<';
        }
        if (c == '>') {
            c = step();
            if (c == '=')  
                return TK_GTE;
            return '>';
        }
        if (c == '-') {
            c = step();
            if (c != '-')
                return '-';
            // else it's a comment
            step_until("\n\r");
            continue;
        }
        // assign or eual
        if (c == '=') {
            c = step();
            if (c != '=')
                return '=';
            step();
            return TK_EQ;
        }
        // ~=
        if (c == '~') {
            c = step();
            if (c != '=')
                return '~';
            step();
            return TK_MATCH;
        }
        // newline
        if (strchr("\n\r", c)) {
            step();
            continue;
        }
        // skip the whitespaces
        if (isspace(c)) {
            tspaces();
            continue;
        }
        // dots '...'
        if (c == '.') {
            c = step();
            if ( c != '.')
                return '.';
            c = step();
            if ( c != '.')
                lex_error("unkown token: '..'");
            step();
            return TK_DOTS;
        }
        // single character tokens
        if (strchr("=,.+:;*/%#{}()[]", c)) {
            step();
            return c;
        }
        // string
        if (strchr("\"\'", c)) {
            tstring(c);
            return TK_STRING;
        }
        //
        if (isdigit(c)) {
            tnumber();
            return TK_NUMBER;
        }
        // keyword
        if (isalpha(c)) {
            tname();
            ri = kwtab->find(_ahead.buf);
            if (ri != kwtab->end()) {
                return ri->second;
            }
            return TK_NAME;
        }
        lex_error("unkown token: %c", c);
    }
    return 0;
}

/* --------------------------------------------------- */

char Lexer::tspaces() {
    char c = _ch;

    if (!isspace(c)) 
        return c;
    while (isspace(c)) {
        c = step();
    }
    return c;
}

char Lexer::tstring(char qc) {
    char c = _ch;

    while ((c = step()) != qc) {
        switch(c) {
        case EOF:
        case '\0':
        case '\n':
        case '\r':
            lex_error("unfinished string");
        case '\\':
            switch(c = step()) {
            case 'n': consume('\n'); break;
            case 'r': consume('\r'); break;
            case 'a': consume('\a'); break;
            case 'b': consume('\b'); break;
            case 'f': consume('\f'); break;
            case 'v': consume('\v'); break;
            case '\\': consume('\\'); break;
            case '\"': consume('\"'); break;
            case '\'': consume('\''); break;
            case '\?': consume('\?'); break;
            // go through
            case '\n':
            case '\r': consume('\n'); break;
            }
        default:
            consume(c);
        }
    }
    step();
    return c;
}

char Lexer::tname(){
    char c = _ch;

    if (!isalpha(c))  
        return c;
    while(isalpha(c) || isdigit(c)){
        consume(c);
        c = step();
    }
    return c;
}

char Lexer::tnumber(){
    char c = _ch;

    c = tdigits();
    // if it's an decimal
    if (c == '.') {
        consume(c);
        c = tdigits();
    }
    return c;
}

char Lexer::tdigits() {
    char c = _ch;
    
    if (!isdigit(c)) {
        lex_error("number expected, but got: %c", c);
    }
    while (isdigit(c)) {
        consume(c);
        c = step();
    }
    if (isalpha(c)) {
        lex_error("malformed number near %s, expected number, but got: %c", _ahead.buf.c_str(), c);
    }
    return c;
}
