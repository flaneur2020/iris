#include <cstring>
#include "token.h"

using namespace iris;

const char* iris::tk2str(int t){
    char *str;
    // if it's an single-character token
    static char tkmem[] = ",\0-\0.\0^\0<\0>\0\0+\0:\0;\0*\0/\0%\0#\0{\0}\0(\0)\0[\0]\0=\0";
    if (str = (char*)memchr(tkmem, t, sizeof(tkmem))) 
        return str;
    // if it's an non-terminal token
    switch (t){
    case TK_DOTS: return "...";
    case TK_MATCH: return "~=";
    case TK_GTE: return ">=";
    case TK_LTE: return "<=";
    case TK_EQ: return "==";
    case TK_NEWLINE: return "NEWLINE";
    case TK_NUMBER: return "<number>";
    case TK_STRING: return "<string>";
    case TK_NAME:  return "<name>";
    case TK_TRUE: return "true";
    case TK_FALSE: return "false";
    case TK_AND: return "and";
    case TK_OR: return "or";
    case TK_NOT: return "not";
    case TK_IF: return "if"; 
    case TK_THEN: return "then"; 
    case TK_NIL: return "nil";
    case TK_FOR: return "for";
    case TK_WHILE: return "while";
    case TK_BREAK: return "break";
    case TK_CONTINUE: return "continue";
    case TK_LOCAL: return "local";
    case TK_FUNCTION: return "function";
    case TK_DO: return "do";
    case TK_END: return "end";
    case TK_REPEAT: return "repeat";
    case TK_ELSE: return "else";
    case TK_ELSE_IF: return "elseif";
    case TK_IN: return "in";
    case TK_RETURN: return "return";
    default: return "<unkown token>";
    }
}
