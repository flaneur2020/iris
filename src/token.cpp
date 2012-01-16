#include "token.h"

const char* tk2str(int t){
    switch (t){
    case '.': return ".";
    case '+': return "+";
    case '-': return "-";
    case '*': return "*";
    case '/': return "/";
    case ':': return ":";
    case ',': return ",";
    case '>': return ">";
    case '<': return "<";
    case '{': return "{";
    case '}': return "}";
    case '(': return "(";
    case ')': return ")";
    case ';': return ";";
    case TK_NEWLINE: return "NEWLINE";
    case TK_GTE: return "GTE";
    case TK_LTE: return "LTE";
    case TK_NUMBER: return "<number>";
    case TK_STRING: return "<string>";
    case TK_NAME:  return "<name>";
    case TK_AND: return "and";
    case TK_OR: return "or";
    case TK_IF: return "if"; 
    case TK_NIL: return "nil";
    case TK_FOR: return "for";
    case TK_WHILE: return "while";
    case TK_BREAK: return "break";
    case TK_CONTINUE: return "continue";
    case TK_LOCAL: return "local";
    case TK_FUNCTION: return "function";
    default: return "<unkown token>";
    }
}
