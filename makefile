CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -O2
all: minilangpp
minilangpp: main.c lexer.c parser.c
$(CC) $(CFLAGS) -o minilangpp main.c lexer.c parser.c
clean:
rm -f minilangpp