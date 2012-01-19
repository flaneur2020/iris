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
    void parse_error(const char *fmt, ...);

private:
    void token(int tk);
    void chunk();
    void block();
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
    void prefix_exp();
    void table_literal();
    void field();
    void fieldlist();
    void func();
    void var_or_exp();
    void func_body();
    void parlist1();
    void namelist();

    int test_lookahead(int tk) const;
    int test_lookahead_n(int arg0, ...) const;
    int test_chunk() const;
    int test_stat() const;
    int test_laststat() const;
    int test_var() const;
    int test_varlist() const;
    int test_var_suffix() const;
    int test_name_and_args() const;
    int test_exp() const;
    int test_explist() const;
    int test_parlist1() const;
    int test_args() const;
    int test_prefix_exp() const;
    int test_table_literal() const;
    int test_field() const;
    int test_fieldlist() const;
    int test_func() const;
    int test_func_call() const;
    int test_func_body() const;
    int test_unop() const;
    int test_var_or_exp() const;
    int test_namelist() const;

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
