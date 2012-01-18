#include <cstdio>
#include <cstdarg>
#include "parser.h"

using namespace iris;

Parser::Parser(FILE *file, const char *file_name)
    : _file(file)
    , _file_name(file_name)
    , _lexer(file, file_name)
{
}

Parser::~Parser(){
}

/* --------------------------------------------------- */

void Parser::parse(){
    chunk();
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

/*
 * stat :  varlist1 '=' explist1 | 
 *	functioncall | 
 *	'do' block 'end' | 
 *	'while' exp 'do' block 'end' | 
 *	'repeat' block 'until' exp | 
 *	'if' exp 'then' block ('elseif' exp 'then' block)* ('else' block)? 'end' | 
 *	'for' NAME '=' exp ',' exp (',' exp)? 'do' block 'end' | 
 *	'for' namelist 'in' explist1 'do' block 'end' | 
 *	'function' funcname funcbody | 
 *	'local' 'function' NAME funcbody | 
 *	'local' namelist ('=' explist1)? ;
 *
 * */
int Parser::stat(){
    if (test_varlist() != P_NOT_MATCH) {
        varlist();
        token('=');
        explist();
    }
}

int Parser::test_stat(){
    if (test_varlist() || test_func_call()) {
        return P_MATCH;
    }
    switch(_lexer.lookahead()->token) {
    case TK_DO:
    case TK_WHILE:
    case TK_REPEAT:
    case TK_IF:
    case TK_FOR:
    case TK_FUNCTION:
    case TK_LOCAL: 
        return P_MATCH;
    }
    return P_NOT_MATCH;
}

int Parser::laststat(){
    if (test_lookahead(TK_RETURN)) {
    }
}

/*
 * varlist : var (',' var)*;
 * */
int Parser::varlist() {
    var();
    while(test_lookahead(',')) {
        token(',');
        var();
    }
    return 0;
}

int Parser::test_varlist() const {
    if (test_var() == P_NOT_MATCH) 
        return P_NOT_MATCH;
    return P_MATCH;
}

// var: (NAME | '(' exp ')' varSuffix) varSuffix*;
int Parser::var() {
    if (test_lookahead(TK_NAME)) {
        token(TK_NAME);
    }
    else if (test_lookahead('(')) {
        token('(');
        exp();
        token(')');
        var_suffix();
    }
    while(test_var_suffix()) {
        var_suffix();
    }
}

int Parser::test_var() {
    if (! test_lookahead_n(2, TK_NAME, '(')) {
        return P_NOT_MATCH;
    }
    return P_MATCH;
}

// varSuffix: nameAndArgs* ('[' exp ']' | '.' NAME);
int Parser::var_suffix() {
    while(test_name_and_args()) {
        name_and_args();
    }
    if (test_lookahead('[')) {
        token('[');
        exp();
        token(']');
        return P_MATCH;
    }
    if (test_lookahead('.')) {
        token('.');
        token(TK_NAME);
        return P_MATCH;
    }
    parse_error("bad var suffix");
}

int Parser::test_var_suffix() {
    if (test_name_and_args()) {
        return P_MATCH;
    }
    if (test_lookahead_n(2, '[', '.')) {
        return P_MATCH;
    }
    return P_NOT_MATCH;
}

/*
 * nameAndArgs: (':' NAME)? args;
 * */
int Parser::name_and_args() {
    if (test_lookahead(':')) {
        token(':');
        token(TK_NAME);
    } 
    args();
}

int Parser::test_name_and_args() const {
    if (test_lookahead(':')) 
        return P_MATCH;
    if (test_args())
        return P_MATCH;
    return P_NOT_MATCH;
}

// args :  '(' (explist1)? ')' | tableconstructor | string ;
int Parser::args(){
    if (test_lookahead('(')) {
        token('(');
        if (test_explist()) 
            explist();
        token(')');
        return P_MATCH;
    }
    if (test_table_constructor()) {
        table_constructor();
        return P_MATCH;
    }
    if (test_lookahead(TK_STRING)) {
        token(TK_STRING);
        return P_MATCH;
    }

        
}

int Parser::test_args() {
    if (test_lookahead_n(2, '(', TK_STRING))
        return P_MATCH;
    if (test_table_constructor())
        return P_MATCH;
    return P_NOT_MATCH;
}

/*
 * exp :  ('nil' | 'false' | 'true' | number | string | '...' | function | prefixexp | tableconstructor | unop exp) (binop exp)* ;
 * */
int Parser::exp(){
}

int Parser::test_exp() {
}

/* --------------------------------------------------- */

int Parser::test_lookahead(int tk) const {
    if (_lexer.lookahead()->token == tk) {
        return P_MATCH;
    }
    return P_NOT_MATCH;
}

int Parser::test_lookahead_n(int n, ...) const {
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


