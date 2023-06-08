CC = gcc
flags = -Wall
filename = main

all: main

main: $(filename).c
	$(CC) $(flags) $(filename).c -o main

test: main
	./main
	wc --bytes *

clean:
	ls --hide=Makefile --hide=main.c | xargs -d '\n' rm
