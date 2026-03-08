
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
static int process_file(const char* path) {
 char* src = read_entire_file(path);
 if (!src) {
 fprintf(stderr, "[%s] No se pudo leer el archivo.\n", path);
 return 2;
 }
 Parser ps;
 parser_init(&ps, src, path);
 printf("====================================================\n");
 printf("Archivo: %s\n", path);
 printf("----------------------------------------------------\n");
 int exit_code = 0;
 if (parse_program(&ps)) {
 printf("✅ Análisis sintáctico exitoso: el programa es válido.\n");
 } else {
 printf("❌ Se encontraron errores. Revisa los mensajes en consola.\n");
 exit_code = 1;
 }
 free(src);
 return exit_code;
}
int main(int argc, char** argv) {
 if (argc < 2) {
 return process_file("tests/test1_ok.min");
 }
 int final_code = 0;
 for (int i = 1; i < argc; i++) {
 int code = process_file(argv[i]);
 if (code != 0) final_code = code;
 }
 return final_code;
}