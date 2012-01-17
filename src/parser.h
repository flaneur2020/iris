#ifndef PARSER_H
#define PARSER_H

namespace iris {

class Parser {
    Lexer lexer;

public:
    explicit Parser();
    ~Parser();

    void parse();

private:
    void stat();
    void if_stat();
    void while_stat();
}

}

#endif
