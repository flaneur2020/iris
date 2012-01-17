#include <cstdio>
#include "parser.h"

using namespace iris;

Parser::Parser(FILE *file, char *file_name)
    : _file(file)
    , _file_name(file_name)
    , _lexer(file, file_name)
{
}

Parser::~Parser(){
    if (_file)
        fclose(_file);
}

/* --------------------------------------------------- */

void Parser::parse(){
    
}
