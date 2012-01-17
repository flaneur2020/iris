#include <cstdio>
#include <cstdarg>
#include "parser.h"

using namespace iris;

Parser::Parser(FILE *file, char *file_name)
    : _file(file)
    , _file_name(file_name)
    , _lexer(file, file_name)
{
}

Parser::~Parser(){
}

/* --------------------------------------------------- */

void Parser::parse(){
}

void Parser::parse_error(char *fmt, ...){
    va_list vp;
    va_start(vp, fmt); 
    fprintf(stderr, "Parse error: ");
    vfprintf(stderr, fmt, vp);
    fprintf(stderr, "\n");
    va_end(vp);
    throw parse_exception();
}

/* --------------------------------------------------- */

int Parser::token(int tk){
    if (_lexer.current()->token != tk) 
        parse_error("%s expected, but got: %s", tk2str(tk), tk2str(_lexer.current()->token));
    _lexer.next();
    return P_MATCH;
}

// chunk : (stat (';')?)* (laststat (';')?)?;
int Parser::chunk(){
    do {
        if (stat() == P_NOT_MATCH) 
            break;
        if (test_lookahead(';')) {
            _lexer.next();
        }
    } while(1);
    return P_MATCH;
}

int Parser::stat(){
}

int Parser::laststat(){
    if (test_lookahead(TK_RETURN)) {
    }
}

int Parser::varlist() {
    return 0;
}

// var: (NAME | '(' exp ')' varSuffix) varSuffix*;
int Parser::var() {
    if (! test_lookahead_n(2, TK_NAME, '(')) {
        return P_NOT_MATCH;
    }

    if (test_lookahead(TK_NAME)) {
        _lexer.next();
    }
    else if (test_lookahead('(')) {
        _lexer.next();
        exp();
    }
}

int Parser::exp(){
}

/* --------------------------------------------------- */

int Parser::test_lookahead(int tk){
    if (_lexer.lookahead()->token == tk) {
        return P_MATCH;
    }
    return P_NOT_MATCH;
}

int Parser::test_lookahead_n(int n, ...) {
    int i;
    va_list vp;
    int arg;
    va_start(vp, n); 
    int r = P_NOT_MATCH;

    for (i = 0; i < n; i++) {
        arg = va_arg(vp, typeof(TK_NIL));
        if (_lexer.lookahead()->token == arg) {
            r = P_MATCH;
        }
    }
    va_end(vp);
    return r;
}

int Parser::test_exp() {
}


