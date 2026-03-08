#include "parser.h"
#include <stdio.h>
static void parse_Prog(Parser* ps);
static void parse_ListaSent(Parser* ps);
static void parse_Sent(Parser* ps);
static void parse_Expr(Parser* ps);
static void parse_ExprP(Parser* ps);
static void parse_Term(Parser* ps);
static void parse_TermP(Parser* ps);
static void parse_Factor(Parser* ps);
static void advance_token(Parser* ps) {
 ps->current = lexer_next(&ps->lx);
}
static void syntax_error(Parser* ps, const char* msg) {
 ps->had_error = true;
 fprintf(stderr,
 "[%s] Error sintáctico en línea %d, columna %d: %s. Token actual: %s
('%s')\n",
 ps->filename ? ps->filename : "<entrada>",
 ps->current.line, ps->current.col,
 msg,
 token_type_name(ps->current.type),
 ps->current.lexeme);
}
static void lex_error(Parser* ps) {
 ps->had_error = true;
 fprintf(stderr,
 "[%s] Error léxico en línea %d, columna %d: caracter no reconocido '%s'\n",
 ps->filename ? ps->filename : "<entrada>",
 ps->current.line, ps->current.col,
 ps->current.lexeme);
}
static void match(Parser* ps, TokenType expected) {
 if (ps->had_error) return;
 if (ps->current.type == TOK_LEX_ERROR) {
 lex_error(ps);
 return;
 }
 if (ps->current.type == expected) {
 advance_token(ps);
 return;
 }
 char msg[256];
 snprintf(msg, sizeof(msg), "se esperaba %s", token_type_name(expected));
 syntax_error(ps, msg);
}
void parser_init(Parser* ps, const char* source, const char* filename) {
 lexer_init(&ps->lx, source);
 ps->filename = filename;
 ps->had_error = false;
 advance_token(ps);
}
bool parse_program(Parser* ps) {
 parse_Prog(ps);
 if (!ps->had_error && ps->current.type != TOK_EOF) {
 syntax_error(ps, "se esperaba fin de archivo (EOF)");
 }
 return !ps->had_error;
}
// Prog -> ListaSent
// ListaSent -> Sent ListaSent | ε
// Sent -> ID '=' Expr ';'
//
// Expr -> Term ExprP
// ExprP -> ('+' Term ExprP) | ('-' Term ExprP) | ε
// Term -> Factor TermP
// TermP -> ('*' Factor TermP) | ('/' Factor TermP) | ε
// Factor -> '(' Expr ')' | ID | NUM
static void parse_Prog(Parser* ps) {
 parse_ListaSent(ps);
}
static void parse_ListaSent(Parser* ps) {
 while (!ps->had_error) {
 if (ps->current.type == TOK_LEX_ERROR) { lex_error(ps); return; }
 if (ps->current.type == TOK_ID) {
 parse_Sent(ps);
 } else {
 return; // ε
 }
 }
}
static void parse_Sent(Parser* ps) {
 match(ps, TOK_ID);
 match(ps, TOK_ASSIGN);
 parse_Expr(ps);
 match(ps, TOK_SEMI);
}
static void parse_Expr(Parser* ps) {
 parse_Term(ps);
 parse_ExprP(ps);
}
static void parse_ExprP(Parser* ps) {
 while (!ps->had_error) {
 if (ps->current.type == TOK_PLUS) {
 match(ps, TOK_PLUS);
 parse_Term(ps);
 } else if (ps->current.type == TOK_MINUS) {
 match(ps, TOK_MINUS);
 parse_Term(ps);
 } else {
 return;
 }
 }
}
static void parse_Term(Parser* ps) {
 parse_Factor(ps);
 parse_TermP(ps);
}
static void parse_TermP(Parser* ps) {
 while (!ps->had_error) {
 if (ps->current.type == TOK_STAR) {
 match(ps, TOK_STAR);
 parse_Factor(ps);
 } else if (ps->current.type == TOK_SLASH) {
 match(ps, TOK_SLASH);
 parse_Factor(ps);
 } else {
 return;
 }
 }
}
static void parse_Factor(Parser* ps) {
 if (ps->had_error) return;
 if (ps->current.type == TOK_LEX_ERROR) { lex_error(ps); return; }
 if (ps->current.type == TOK_LPAREN) {
 match(ps, TOK_LPAREN);
 parse_Expr(ps);
 match(ps, TOK_RPAREN);
 return;
 }
 if (ps->current.type == TOK_ID) { match(ps, TOK_ID); return; }
 if (ps->current.type == TOK_NUM) { match(ps, TOK_NUM); return; }
 syntax_error(ps, "se esperaba un Factor: '(' o ID o NUM");
}