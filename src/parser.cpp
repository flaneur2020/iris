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

void Parser::parse_error(const char *fmt, ...) {
    va_list vp;
    va_start(vp, fmt); 
    fprintf(stderr, "Parse error: %s: line %d: col: %d\n", _file_name, _lexer.line(), _lexer.col());
    vfprintf(stderr, fmt, vp);
    fprintf(stderr, ", but got '%s' %s\n", tk2str(_lexer.lookahead()->token), _lexer.lookahead()->buf.c_str());
    va_end(vp);
    throw parse_exception();
}

/* --------------------------------------------------- */

// on tk == 0, any token is matched.
void Parser::token(int tk){
    if ((tk != 0) && (!test_lookahead(tk)))  {
        parse_error("bad token. expected '%s'", tk2str(tk));
    }
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

// block : chunk
void Parser::block() {
    chunk();
}

/*
 * exp :  ('nil' | 'false' | 'true' | number | string | '...' | function | prefixexp | tableconstructor | unop exp) (binop exp)* ;
 * */
void Parser::exp(){
    if (test_lookahead_n(TK_NIL, TK_FALSE, TK_TRUE, TK_NUMBER, TK_STRING, TK_DOTS, 0)) {
        token(0);
    } 
    else if (test_func()) {
        func();
    }
    else if (test_prefix_exp()){
        prefix_exp();
    }
    else if (test_table_literal()) {
        table_literal();
    }
    else if (test_lookahead_n('-', TK_NOT, '#', 0)) {
        token(0);
        exp();
    }
    else {
        parse_error("bad exp. exptected 'nil', 'false', 'true', number, string, '...', function, prefixexp, table literal, or unop exp");
    }

    while (test_unop()) {
        token(0);
        exp();
    }

}

int Parser::test_exp() const {
    return test_lookahead_n(TK_NIL, TK_FALSE, TK_TRUE, TK_NUMBER, TK_STRING, TK_DOTS, 0)
        || test_func()
        || test_prefix_exp()
        || test_table_literal()
        || test_unop();
}

int Parser::test_unop() const { 
    return test_lookahead_n('+' , '-', '*', '/', '^', '%', TK_DOTS, '<', TK_LTE, '>', TK_GTE, TK_EQ, TK_MATCH, TK_AND, TK_OR, 0);
}

// function : 'function' funcbody;
void Parser::func() {
    token(TK_FUNCTION);
    func_body();
}

int Parser::test_func() const {
    return test_lookahead(TK_FUNCTION);
}

// funcname : NAME ('.' NAME)* (':' NAME)? ;
void Parser::func_name() {
    token(TK_NAME);
    while(test_lookahead('.')) {
        token('.');
        token(TK_NAME);
    }
    if (test_lookahead(':')) {
        token(':');
        token(TK_NAME);
    }
}

int Parser::test_func_name() const {
    return test_lookahead_n(TK_NAME, '.', ':', 0);
}


// funcbody : '(' (parlist1)? ')' block 'end';
void Parser::func_body() {
    token('(');
    if (test_parlist1()) {
        parlist1();
    }
    token(')');
    block();
    token(TK_END);
}

int Parser::test_func_body() const {
    return test_lookahead('(');
}

// parlist1 : namelist (',' '...')? | '...';
void Parser::parlist1() {
    if (test_namelist()){
        namelist();
        if (test_lookahead_n(',', TK_DOTS, 0)) {
            token(0);
        }
    }
    else if (test_lookahead(TK_DOTS)) {
        token(0);
    }
    else {
        parse_error("expected namelist or '...'");
    }
}

int Parser::test_parlist1() const {
    return test_namelist() 
        || test_lookahead(TK_DOTS);
}

// namelist : NAME (',' NAME)*;
void Parser::namelist() {
    token(TK_NAME);
    while(test_lookahead(',')) {
        token(',');
        token(TK_NAME);
    }
}

int Parser::test_namelist() const {
    return test_lookahead(TK_NAME);
}

// explist1 : exp (',' exp )*;
void Parser::explist() {
    exp();
    while (test_lookahead(',')) {
        token(',');
        exp();
    }
}

int Parser::test_explist() const {
    return test_exp();
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
    if (test_varlist()) {
        varlist();
        token('=');
        explist();
    }
    else if (test_func_call()) {
        func_call();
    }
    else if (test_lookahead(TK_DO)) {
        token(TK_DO);
        block();
        token(TK_END);
    }
    else if (test_lookahead(TK_WHILE)) {
        token(TK_WHILE);
        exp();
        token(TK_DO);
        block();
        token(TK_END);
    }
    else if (test_lookahead(TK_REPEAT)) {
        token(TK_REPEAT);
        block();
        token(TK_UNTIL);
        exp();
    }
    // 'if' exp 'then' block ('elseif' exp 'then' block)* ('else' block)? 'end' | 
    else if (test_lookahead(TK_IF)) {
        token(TK_IF);
        exp();
        token(TK_THEN);
        block();
        while(test_lookahead(TK_ELSE_IF)) {
            token(TK_ELSE_IF);
            exp();
            token(TK_THEN);
            block();
        }
        if (test_lookahead(TK_ELSE)) {
            token(TK_ELSE);
            block();
        }
        token(TK_END);
    }
    // 'for' NAME '=' exp ',' exp (',' exp)? 'do' block 'end' | 
    // 'for' NAME (, namelist1)? 'in' explist1 'do' block 'end' | 
    else if (test_lookahead(TK_FOR)) {
        token(TK_FOR);
        token(TK_NAME);
        if (test_lookahead('=')) {
            token('=');
            exp();
            token(',');
            exp();
            if (test_lookahead(',')) {
                token(',');
                exp();
            }
            token(TK_DO);
            block();
            token(TK_END);
        }
        else if (test_lookahead(',')) {
            token(',');
            namelist();
            token(TK_IN);
            explist();
            token(TK_DO);
            block();
            token(TK_END);
        }
    }
    // 'function' funcname funcbody | 
    else if (test_lookahead(TK_FUNCTION)) {
        token(TK_FUNCTION);
        func_name();
        func_body();
    }
    // 'local' 'function' NAME funcbody | 
    // 'local' namelist ('=' explist1)? ;
    else if (test_lookahead(TK_LOCAL)) {
        token(TK_LOCAL);
        if (test_lookahead(TK_FUNCTION)) {
            token(TK_NAME);
            func_body();
        }
        else if (test_namelist()) {
            namelist();
            if (test_lookahead('=')) {
                token('=');
                explist();
            }
        }
        else {
            parse_error("bad 'local' statement");
        }
    }
    else {
        parse_error("bad statement");
    }
}

int Parser::test_stat() const {
    return test_varlist()
        || test_func_call()
        || test_lookahead_n(TK_DO, TK_WHILE, TK_REPEAT, TK_IF, TK_FOR, TK_FUNCTION, TK_LOCAL, 0);
}

// laststat : 'return' (explist1)? | 'break';
void Parser::laststat(){
    if (test_lookahead(TK_RETURN)) {
        token(TK_RETURN);
        if (test_explist()) {
            explist();
        }
        return;
    }
    if (test_lookahead(TK_BREAK)) {
        token(TK_BREAK);
        return;
    }
    parse_error("bad laststat. expected 'return' or 'break'");
}

int Parser::test_laststat() const {
    return test_lookahead_n(TK_RETURN, TK_BREAK, 0);
}


// varlist : var (',' var)*;
void Parser::varlist() {
    var();
    while(test_lookahead(',')) {
        token(',');
        var();
    }
    return;
}

int Parser::test_varlist() const {
    return test_var();
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
    else {
        parse_error("bad var.");
    }

    while(test_var_suffix()) {
        var_suffix();
    }
}

int Parser::test_var() const {
    return test_lookahead_n(TK_NAME, '(', 0);
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
    }
    else if (test_lookahead('.')) {
        token('.');
        token(TK_NAME);
    }
    else {
        parse_error("bad var suffix. expected '[' or '.'");
    }
}

int Parser::test_var_suffix() const {
    return (test_name_and_args()) 
        || (test_lookahead_n('[', '.', 0));
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
    return test_lookahead(':') 
        || test_args();
}

// args :  '(' (explist1)? ')' | tableconstructor | string ;
void Parser::args(){
    if (test_lookahead('(')) {
        token('(');
        if (test_explist()) 
            explist();
        token(')');
    }
    else if (test_table_literal()) {
        table_literal();
    }
    else if (test_lookahead(TK_STRING)) {
        token(TK_STRING);
    }
    else {
        parse_error("bad args.");
    }
}

int Parser::test_args() const {
    return test_lookahead_n('(', TK_STRING, 0) 
        || test_table_literal();
}

// tableconstructor : '{' (fieldlist)? '}';
void Parser::table_literal() {
    token('{');
    if (test_fieldlist()) {
        fieldlist();
    }
    token('}');
}

int Parser::test_table_literal() const {
    return test_lookahead('{');
}

// fieldlist : field (fieldsep field)* (fieldsep)?;
void Parser::fieldlist() {
    field();
    while(test_lookahead_n(',', ';', 0)) {
        token(0);
        field();
    }
    if (test_lookahead_n(',', ';', 0)) {
        token(0);
    }
}

int Parser::test_fieldlist() const {
    return test_field();
}

// field : '[' exp ']' '=' exp | NAME '=' exp | exp;
void Parser::field() {
    if (test_lookahead('[')) {
        token('[');
        exp();
        token(']');
        token('=');
        exp();
    }
    else if (test_lookahead(TK_NAME)) {
        token(TK_NAME);
        token('=');
        exp();
    }
    else if (test_exp()) {
        exp();
    }
    else {
        parse_error("bad field.");
    }
}

int Parser::test_field() const {
    return test_lookahead_n('[', TK_NAME, 0)
        || test_exp();
}

// functioncall: varOrExp nameAndArgs+;
void Parser::func_call() {
    var_or_exp();
    do {
        name_and_args();
    } while(test_name_and_args());
}

int Parser::test_func_call() const {
    return test_var_or_exp();
}

// prefixexp: varOrExp nameAndArgs*;
void Parser::prefix_exp() {
    var_or_exp();
    while(test_name_and_args()) {
        name_and_args();
    }
}

int Parser::test_prefix_exp() const {
    return test_var_or_exp();
}

// varOrExp: var | '(' exp ')';
void Parser::var_or_exp() {
    if (test_var()) {
        var();
    }
    else if (test_lookahead('(')) {
        token('(');
        exp();
        token(')');
    }
    else {
        parse_error("bad var or exp. expected var or '('");
    }
}

int Parser::test_var_or_exp() const {
    return test_var()
        || test_lookahead('(');
}

/* --------------------------------------------------- */

int Parser::test_lookahead(int tk) const {
    return (_lexer.lookahead()->token == tk);
}

int Parser::test_lookahead_n(int arg0, ...) const {
    int i;
    va_list vp;
    va_start(vp, arg0); 
    int arg = arg0;
    int t = _lexer.lookahead()->token;

    do {
        // printf("- %s, %d\n", tk2str(arg), arg);
        if (t == arg) {
            va_end(vp);
            return P_MATCH;
        }
    } while ((arg = va_arg(vp, int)) != 0);
    va_end(vp);
    return P_NOT_MATCH;
}


