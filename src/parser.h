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
    int token(int tk);
    int chunk();
    int stat();
    int laststat();
    int ifstat();
    int whilestat();
    int var();
    int varlist();
    int exp();
    int test_lookahead(int tk) const;
    int test_lookahead_n(int n, ...) const;
    int test_exp() const;
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
