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
    fprintf(stderr, "Parse error: line %d: ", _lexer.line());
    vfprintf(stderr, fmt, vp);
    fprintf(stderr, "\n");
    va_end(vp);
    throw parse_exception();
}

/* --------------------------------------------------- */

void Parser::token(int tk){
    if (_lexer.lookahead()->token != tk) 
        parse_error("%s expected, but got: %s: %s", tk2str(tk), tk2str(_lexer.lookahead()->token), _lexer.lookahead()->buf.c_str());
    _lexer.next();
}

// chunk : (stat (';')?)* (laststat (';')?)?;
void Parser::chunk(){
    while (test_stat()) {
        stat();
        if (test_lookahead(';')) {
            token(';');
        }
    }
    if (test_laststat()) {
        laststat();
        if (test_lookahead(';')) {
            token(';');
        }
    }
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
void Parser::stat(){
    if (test_varlist() != P_NOT_MATCH) {
        varlist();
        token('=');
        explist();
    }
}

int Parser::test_stat() const {
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

void Parser::laststat(){
    if (test_lookahead(TK_RETURN)) {
    }
}

// laststat : 'return' (explist1)? | 'break';
int Parser::test_laststat() const {
}

/*
 * varlist : var (',' var)*;
 * */
void Parser::varlist() {
    var();
    while(test_lookahead(',')) {
        token(',');
        var();
    }
    return;
}

int Parser::test_varlist() const {
    if (test_var() == P_NOT_MATCH) 
        return P_NOT_MATCH;
    return P_MATCH;
}

// var: (NAME | '(' exp ')' varSuffix) varSuffix*;
void Parser::var() {
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

int Parser::test_var() const {
    if (test_lookahead_n(2, TK_NAME, '(')) {
        return P_MATCH;
    }
    return P_NOT_MATCH;
}

// varSuffix: nameAndArgs* ('[' exp ']' | '.' NAME);
void Parser::var_suffix() {
    while(test_name_and_args()) {
        name_and_args();
    }
    if (test_lookahead('[')) {
        token('[');
        exp();
        token(']');
        return;
    }
    if (test_lookahead('.')) {
        token('.');
        token(TK_NAME);
        return;
    }
    parse_error("bad var suffix");
}

int Parser::test_var_suffix() const {
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
void Parser::name_and_args() {
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
void Parser::args(){
    if (test_lookahead('(')) {
        token('(');
        if (test_explist()) 
            explist();
        token(')');
        return;
    }
    if (test_table_literal()) {
        table_literal();
        return;
    }
    if (test_lookahead(TK_STRING)) {
        token(TK_STRING);
        return;
    }

        
}

int Parser::test_args() const {
    if (test_lookahead_n(2, '(', TK_STRING))
        return P_MATCH;
    if (test_table_literal())
        return P_MATCH;
    return P_NOT_MATCH;
}

// tableconstructor : '{' (fieldlist)? '}';
void Parser::table_literal() {
}

int Parser::test_table_literal() const {
    return 0;
}

// fieldlist : field (fieldsep field)* (fieldsep)?;
void Parser::fieldlist() {
}

int Parser::test_fieldlist() const {
    return 0;
}

// field : '[' exp ']' '=' exp | NAME '=' exp | exp;
void Parser::field() {
}

int Parser::test_field() const {
    return 0;
}


/*
 * exp :  ('nil' | 'false' | 'true' | number | string | '...' | function | prefixexp | tableconstructor | unop exp) (binop exp)* ;
 * */
void Parser::exp(){
}

int Parser::test_exp() const {
    return 0;
}

// explist1 : (exp ',')* exp;
void Parser::explist() {
}

int Parser::test_explist() const {
    return 0;
}

// functioncall: varOrExp nameAndArgs+;
void Parser::func_call() {
}

int Parser::test_func_call() const {
    return 0;
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
        arg = va_arg(vp, int);
        if (_lexer.lookahead()->token == arg) {
            r = P_MATCH;
        }
    }
    va_end(vp);
    return r;
}


