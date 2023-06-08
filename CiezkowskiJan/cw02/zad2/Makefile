CC = gcc
flags = -Wall
filename = main

all: main_block main_char

test: test_block test_char

test_char: main_char
	./main_char main.c tmp1 ; ./main_char tmp1 tmp2 ; diff -s tmp2 main.c

test_block: main_block
	./main_block main.c tmp1 ; ./main_block tmp1 tmp2 ; diff -s tmp2 main.c

main_block: $(filename).c
	$(CC) $(flags) $(filename).c -D READ_BLOCK=1024 -o main_block

main_char: $(filename).c
	$(CC) $(flags) $(filename).c -o main_char

clean:
	ls --hide=Makefile --hide=main.c --hide=pomiar_zad_2.txt | xargs -d '\n' rm
