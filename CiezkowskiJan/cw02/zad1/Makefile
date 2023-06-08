CC = gcc
flags = -Wall
filename = main

all: main_lib main_sys

test: test_lib test_sys

main_lib: $(filename).c
	$(CC) $(flags) $(filename).c -D USE_LIBRARY -o main_lib
	
main_sys: $(filename).c
	$(CC) $(flags) $(filename).c -o main_sys

test_lib: main_lib
	./main_lib a b main.c test_lib ; tr a b < main.c > tmp ; diff -s tmp test_lib

test_sys: main_sys
	./main_sys a b main.c test_sys ; tr a b < main.c > tmp ; diff -s tmp test_sys
	
clean:
	ls --hide=Makefile --hide=main.c --hide=pomiar_zad_1.txt | xargs -d '\n' rm
