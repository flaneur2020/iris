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
    void p_debug(const char* fmt, ...);

    void token(int tk);
    void chunk();
    void block();
    void stat();
    void last_stat();
    void if_stat();
    void while_stat();
    void repeat_stat();
    void for_stat();
    void local_stat();
    void func_stat();
    void assign_stat();
    void exp();
    void prefix_exp();
    int  primary_exp(); // assignment or func call
    void explist();
    void table_constructor();
    void field();
    void func_body();
    void func_name();
    void func_args();
    void parlist();
    void namelist();

    int test_lookahead(int tk) const;
    int test_lookahead_n(int arg0, ...) const;
    int test_chunk() const;
    int test_stat() const;
    int test_assign_stat() const;
    int test_last_stat() const;
    int test_exp() const;
    int test_explist() const;
    int test_parlist() const;
    int test_prefix_exp() const;
    int test_primary_exp() const;
    int test_table_constructor() const;
    int test_field() const;
    int test_func_body() const;
    int test_func_name() const;
    int test_func_args() const;
    int test_unop() const;
    int test_binop() const;
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

const int P_LVALUE = 0;
const int P_FCALL = 1;

struct parse_exception : std::exception {};

}

#endif
