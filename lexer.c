
#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char peek(const Lexer* lx) {
 return lx->src[lx->pos];
}
static char advance(Lexer* lx) {
 char c = lx->src[lx->pos];
 if (c == '\0') return c;
 lx->pos++;
 if (c == '\n') {
 lx->line++;
 lx->col = 1;
 } else {
 lx->col++;
 }
 return c;
}
static void skip_ws(Lexer* lx) {
 while (isspace((unsigned char)peek(lx))) {
 advance(lx);
 }
}
const char* token_type_name(TokenType t) {
 switch (t) {
 case TOK_ID: return "ID";
 case TOK_NUM: return "NUM";
 case TOK_PLUS: return "+";
 case TOK_MINUS: return "-";
 case TOK_STAR: return "*";
 case TOK_SLASH: return "/";
 case TOK_ASSIGN: return "=";
 case TOK_SEMI: return ";";
 case TOK_LPAREN: return "(";
 case TOK_RPAREN: return ")";
 case TOK_EOF: return "EOF";
 case TOK_LEX_ERROR: return "LEX_ERROR";
 default: return "UNKNOWN";
 }
}
void lexer_init(Lexer* lx, const char* source) {
 lx->src = source ? source : "";
 lx->pos = 0;
 lx->line = 1;
 lx->col = 1;
}
static Token make_token(TokenType type, const char* lexeme, int line, int col) {
 Token t;
 t.type = type;
 t.line = line;
 t.col = col;
 t.lexeme[0] = '\0';
 if (lexeme) {
 strncpy(t.lexeme, lexeme, sizeof(t.lexeme) - 1);
 t.lexeme[sizeof(t.lexeme) - 1] = '\0';
 }
 return t;
}
Token lexer_next(Lexer* lx) {
 skip_ws(lx);
 int start_line = lx->line;
 int start_col = lx->col;
 char c = peek(lx);
 if (c == '\0') {
 return make_token(TOK_EOF, "", start_line, start_col);
 }
 if (isdigit((unsigned char)c)) {
 char buf[128];
 int i = 0;
 while (isdigit((unsigned char)peek(lx)) && i < (int)sizeof(buf) - 1) {
 buf[i++] = advance(lx);
 }
 buf[i] = '\0';
 return make_token(TOK_NUM, buf, start_line, start_col);
 }
 if (isalpha((unsigned char)c) || c == '_') {
 char buf[128];
 int i = 0;
 while ((isalnum((unsigned char)peek(lx)) || peek(lx) == '_') && i <
(int)sizeof(buf) - 1) {
 buf[i++] = advance(lx);
 }
 buf[i] = '\0';
 return make_token(TOK_ID, buf, start_line, start_col);
 }
 advance(lx);
 switch (c) {
 case '+': return make_token(TOK_PLUS, "+", start_line, start_col);
 case '-': return make_token(TOK_MINUS, "-", start_line, start_col);
 case '*': return make_token(TOK_STAR, "*", start_line, start_col);
 case '/': return make_token(TOK_SLASH, "/", start_line, start_col);
 case '=': return make_token(TOK_ASSIGN, "=", start_line, start_col);
 case ';': return make_token(TOK_SEMI, ";", start_line, start_col);
 case '(': return make_token(TOK_LPAREN, "(", start_line, start_col);
 case ')': return make_token(TOK_RPAREN, ")", start_line, start_col);
 default: {
 char errlex[4] = {0};
 errlex[0] = c;
 return make_token(TOK_LEX_ERROR, errlex, start_line, start_col);
 }
 }
}
char* read_entire_file(const char* path) {
 FILE* f = fopen(path, "rb");
 if (!f) return NULL;
 if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
 long sz = ftell(f);
 if (sz < 0) { fclose(f); return NULL; }
 rewind(f);
 char* buf = (char*)malloc((size_t)sz + 1);
 if (!buf) { fclose(f); return NULL; }
 size_t n = fread(buf, 1, (size_t)sz, f);
 fclose(f);
 buf[n] = '\0';
 return buf;
}