#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

namespace iris {

class Parser {
    FILE *_file;
    const char *_file_name;
    Lexer _lexer;

public:
    Parser(FILE *file, const char *file_name);
    ~Parser();

    void parse();
    void parse_error(char *fmt, ...);

private:
    void token(int tk);
    void chunk();
    void stat();
    void laststat();
    void ifstat();
    void whilestat();
    void var();
    void varlist();
    void var_suffix();
    void name_and_args();
    void exp();
    void explist();
    void args();
    void func_call();
    void table_literal();
    void field();
    void fieldlist();

    int test_lookahead(int tk) const;
    int test_lookahead_n(int n, ...) const;
    int test_chunk() const;
    int test_stat() const;
    int test_laststat() const;
    int test_var() const;
    int test_varlist() const;
    int test_var_suffix() const;
    int test_name_and_args() const;
    int test_exp() const;
    int test_explist() const;
    int test_args() const;
    int test_func_call() const;
    int test_table_literal() const;
    int test_field() const;
    int test_fieldlist() const;

};

/* 
 * for each rule: 
 * if the first input did not match the rule, returns P_NOT_MATCH, 
 * nothing is consumed, else returns P_MATCH;
 * if the input match the rule, but the further input do not 
 * match it any more, raise an exception.
 * 
 * */
const int P_NOT_MATCH = 0;
const int P_MATCH = 1;

struct parse_exception : std::exception {};

}

#endif
