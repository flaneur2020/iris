#include <cstdio>
#include <cstdarg>
#include "iris.h"
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
    fprintf(stderr, ", but got '%s' %s\n", tk2str(_lexer.lookahead().token), _lexer.lookahead().buf.c_str());
    va_end(vp);
    throw parse_exception();
}

/* --------------------------------------------------- */

// on tk == 0, any token is matched.
void Parser::token(int tk){
    p_debug("> token %s\n", tk2str(tk));
    if ((tk != 0) && (!test_lookahead(tk)))  {
        parse_error("bad token. expected '%s'", tk2str(tk));
    }
    _lexer.next();
}

// chunk : (stat (';')?)* (last_stat (';')?)?;
void Parser::chunk(){
    p_debug("> chunk\n");
    while (test_stat()) {
        stat();
        if (test_lookahead(';')) {
            token(';');
        }
    }
    if (test_last_stat()) {
        last_stat();
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
 * stat :  
 *	lvalue_or_fcall | -- assignment or func call
 *	'do' block 'end' | 
 *	'while' exp 'do' block 'end' | 
 *	'repeat' block 'until' exp | 
 *	'if' exp 'then' block ('elseif' exp 'then' block)* ('else' block)? 'end' | 
 *	'for' NAME '=' exp ',' exp (',' exp)? 'do' block 'end' | 
 *	'for' namelist 'in' explist 'do' block 'end' | 
 *	'function' funcname funcbody | 
 *	'local' 'function' NAME funcbody | 
 *	'local' namelist ('=' explist)? |
 *
 * */
void Parser::stat(){
    int t;
    int t2;

    p_debug("> stat\n");
    p_debug("\t ahead(): %s\n", _lexer.lookahead().buf.c_str());
    // assignment or function call
    if (test_lvalue_or_fcall()) {
        t = lvalue_or_fcall();
        if (t == P_LVALUE) {
            p_debug("lvalue\n");
            while (test_lookahead(',')) {
                t2 = lvalue_or_fcall();
                if (t2 != P_LVALUE) {
                    parse_error("bad assignment, expected left value");
                }
            }
            token('=');
            explist();
        }
        else if (t == P_FCALL) {
            p_debug("func call\n");
        }
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
    // 'for' NAME (, namelist1)? 'in' explist 'do' block 'end' | 
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
        else {
            parse_error("bad for statment");
        }
    }
    // 'function' funcname funcbody | 
    else if (test_lookahead(TK_FUNCTION)) {
        token(TK_FUNCTION);
        func_name();
        func_body();
    }
    // 'local' 'function' NAME funcbody | 
    // 'local' namelist ('=' explist)? ;
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
    else if (test_exp()) {
        parse_error("bad statement");
    }
    else {
        parse_error("bad statement");
    }
}

int Parser::test_stat() const {
    return test_lvalue_or_fcall()
        || test_lookahead_n(TK_DO, TK_WHILE, TK_REPEAT, TK_IF, TK_FOR, TK_FUNCTION, TK_LOCAL, 0)
        || test_exp();
}

// last_stat : 'return' (explist)? | 'break';
void Parser::last_stat(){
    p_debug("> last_stat");
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
    parse_error("bad last_stat. expected 'return' or 'break'");
}

int Parser::test_last_stat() const {
    return test_lookahead_n(TK_RETURN, TK_BREAK, 0);
}

/*
 * exp :  ('nil' | 'false' | 'true' | number | string | '...' | function | prefix_exp | lvalue_or_fcall | tableconstructor | unop exp | ) (binop exp)* ;
 * */
void Parser::exp(){
    p_debug("> exp\n");
    if (test_lookahead_n(TK_NIL, TK_FALSE, TK_TRUE, TK_NUMBER, TK_STRING, TK_DOTS, 0)) {
        token(0);
    } 
    else if (test_lookahead(TK_FUNCTION)) {
        token(TK_FUNCTION);
        func_body();
    }
    else if (test_lookahead('(')) {
        prefix_exp();
    }
    else if (test_lvalue_or_fcall()){
        lvalue_or_fcall();
    }
    else if (test_table_constructor()) {
        table_constructor();
    }
    else if (test_unop()) {
        token(0);
        exp();
    }
    else {
        parse_error("bad exp. exptected 'nil', 'false', 'true', number, string, '...', function, lvalue, function call, table literal, or unop exp");
    }

    while (test_binop()) {
        token(0);
        exp();
    }

}

int Parser::test_exp() const {
    return test_lookahead_n(TK_NIL, TK_FUNCTION, TK_FALSE, TK_TRUE, TK_NUMBER, TK_STRING, TK_DOTS, 0)
        || test_prefix_exp()
        || test_table_constructor()
        || test_unop();
}

int Parser::test_unop() const {
    return test_lookahead_n('-', TK_NOT, '#');
}

int Parser::test_binop() const { 
    return test_lookahead_n('+' , '-', '*', '/', '^', '%', TK_DOTS, '<', TK_LTE, '>', TK_GTE, TK_EQ, TK_MATCH, TK_AND, TK_OR, 0);
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


// funcbody : '(' (parlist)? ')' block 'end';
void Parser::func_body() {
    token('(');
    if (test_parlist()) {
        parlist();
    }
    token(')');
    block();
    token(TK_END);
}

int Parser::test_func_body() const {
    return test_lookahead('(');
}

// parlist : namelist (',' '...')? | '...';
void Parser::parlist() {
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

int Parser::test_parlist() const {
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

// explist : exp (',' exp )*;
void Parser::explist() {
    p_debug("> explist\n");
    exp();
    while (test_lookahead(',')) {
        token(',');
        exp();
    }
}

int Parser::test_explist() const {
    return test_exp();
}

// func_args :  '(' (explist)? ')' | tableconstructor | string ;
void Parser::func_args(){
    p_debug("> func_args\n");
    if (test_lookahead('(')) {
        token('(');
        if (test_explist()) 
            explist();
        token(')');
    }
    else if (test_table_constructor()) {
        table_constructor();
    }
    else if (test_lookahead(TK_STRING)) {
        token(TK_STRING);
    }
    else {
        parse_error("bad func_args.");
    }
}

int Parser::test_func_args() const {
    return test_lookahead_n('(', TK_STRING, 0) 
        || test_table_constructor();
}

// tableconstructor : '{' (field ((','|';') field)* (','|';')?)? '}';
void Parser::table_constructor() {
    p_debug("> table_constructor");
    token('{');
    if (test_field()) {
        field();
        while(test_lookahead_n(',', ';', 0)) {
            token(0);
            if (! test_field())
                break;
            field();
        }
    }
    token('}');
}

int Parser::test_table_constructor() const {
    return test_lookahead('{');
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

// lvalue_or_fcall: prefix_exp ( '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs )*
int Parser::lvalue_or_fcall() {
    p_debug("> lvalue_or_fcall\n");
    int t = P_LVALUE;

    prefix_exp();
    while (test_lookahead_n('.', '[', ':', 0) || test_func_args()) {
        if (test_lookahead('.')) {
            token('.');
            token(TK_NAME);
            t = P_LVALUE;
        }
        else if (test_lookahead('[')) {
            token('[');
            exp();
            token(']');
            t = P_LVALUE;
        }
        else if (test_lookahead(':')) {
            token(':');
            token(TK_NAME);
            func_args();
            t = P_FCALL;
        }
        else if (test_func_args()) {
            func_args();
            t = P_FCALL;
        }
    }
    return t;
}

int Parser::test_lvalue_or_fcall() const {
    return test_prefix_exp();
} 

// prefix_exp: NAME | '(' exp ')'
void Parser::prefix_exp() {
    p_debug("> prefix_exp\n");
    if (test_lookahead(TK_NAME)) {
        token(TK_NAME);
    }
    else if (test_lookahead('(')) {
        token('(');
        exp();
        token(')');
    }
    else {
        parse_error("bad prefixexp");
    }
}

int Parser::test_prefix_exp() const {
    return test_lookahead_n(TK_NAME, '(', 0);
}

/* --------------------------------------------------- */

int Parser::test_lookahead(int tk) const {
    return (_lexer.lookahead().token == tk);
}

int Parser::test_lookahead_n(int arg0, ...) const {
    int i;
    va_list vp;
    va_start(vp, arg0); 
    int arg = arg0;
    int t = _lexer.lookahead().token;

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

void Parser::p_debug(const char* fmt, ...) {
    #ifdef NDEBUG
    va_list vp;
    va_start(vp, fmt); 
    vfprintf(stderr, fmt, vp);
    fprintf(stderr, "\t %s\n", _lexer.lookahead().buf.c_str());
    va_end(vp);
#endif
}

