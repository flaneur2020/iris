#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include "proto.h"

typedef struct IrParser {
    IrVM *vm;
    IrLex lex;
} IrParser;

#endif
