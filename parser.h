#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include <stdbool.h>
typedef struct {
 Lexer lx;
 Token current;
 bool had_error;
 const char* filename;
} Parser;
void parser_init(Parser* ps, const char* source, const char* filename);
bool parse_program(Parser* ps);
#endif