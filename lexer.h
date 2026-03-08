#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <stddef.h>
typedef struct {
 const char* src; // buffer completo
 size_t pos; // índice actual
 int line; // línea actual (1..)
 int col; // columna actual (1..)
} Lexer;
void lexer_init(Lexer* lx, const char* source);
Token lexer_next(Lexer* lx);
// Utilidad: cargar archivo a memoria (caller debe free())
char* read_entire_file(const char* path);
#endif