#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include "lexer.h"
#include "token.h"

using namespace std;
using namespace iris;

/* ----------------------------------------------------- */

static map<string, int> *kwtab = NULL;

Lexer::Lexer(FILE *file, const char *file_name)
    : file(file)
    , file_name(file_name)
    , current()
    , ahead()
    , line(0)
    , col(0)
    , ch('\0')
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

int Lexer::next() {
    this->current.token = this->ahead.token;
    this->current.buf   = this->ahead.buf;
    this->ahead.token = lex();
    return this->current.token;
}

int Lexer::lookahead() {
    return this->ahead.token;
}

/* ----------------------------------------------------- */

char Lexer::step() {
    char c;

    c = next_char();
    if (c == '\n') {
        this->line++;
        this->col = 0;
    }
    else {
        this->col++;
    }
    return (this->ch = c);
}

char Lexer::step_until(const char *str) {
    while (! strchr(str, step()));
    return '\0';
}

char Lexer::consume(char c){
    this->ahead.buf.push_back(c);
    return c;
}

void Lexer::reset_buf(){
    this->ahead.buf.clear();
}

char Lexer::next_char() {
    return fgetc(this->file);
}

int Lexer::lex() {
    char c;
    int r;
    int tk;
    map<string, int>::iterator ri;

    while((c = this->ch) != EOF && c != '\0') {
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
        // newline
        if (strchr("\n\r", c)) {
            step();
            return TK_NEWLINE;
        }
        // skip the whitespaces
        if (isspace(c)) {
            tspaces();
            continue;
        }
        // single character tokens
        if (strchr(",.+:*/{}()", c)) {
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
            ri = kwtab->find(ahead.buf);
            if (ri != kwtab->end()) {
                return ri->second;
            }
            return TK_NAME;
        }
        lex_error(this, "unkown token: %c", c);
    }
    return 0;
}

/* --------------------------------------------------- */

char Lexer::tspaces() {
    char c = this->ch;

    if (!isspace(c)) 
        return c;
    while (isspace(c)) {
        c = step();
    }
    return c;
}

char Lexer::tstring(char qc) {
    char c = this->ch;

    while ((c = step()) != qc) {
        switch(c) {
        case EOF:
        case '\0':
        case '\n':
        case '\r':
            lex_error(this, "unfinished string");
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
    char c = this->ch;

    if (!isalpha(c))  
        return c;
    while(isalpha(c) || isdigit(c)){
        consume(c);
        c = step();
    }
    return c;
}

char Lexer::tnumber(){
    char c = this->ch;

    c = tdigits();
    // if it's an decimal
    if (c == '.') {
        consume(c);
        c = tdigits();
    }
    return c;
}

char Lexer::tdigits() {
    char c = this->ch;
    
    if (!isdigit(c)) 
        lex_error(this, "number expected, but got: %c", c);
    while (isdigit(c)) {
        consume(c);
        c = step();
    }
    if (isalpha(c)) {
        lex_error(this, "malformed number near %s, expected number, but got: %c", this->ahead.buf, c);
    }
    return c;
}
