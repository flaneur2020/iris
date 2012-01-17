#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

namespace iris {

class Parser {
    FILE *_file;
    char *_file_name;
    Lexer _lexer;

public:
    Parser(FILE *file, char *file_name);
    ~Parser();

    void parse();

private:
    void stat();
    void if_stat();
    void while_stat();
};

}

#endif
