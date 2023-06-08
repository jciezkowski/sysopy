CC = gcc
flags = -Wall
filename = main

all: main

main: main.c
	$(CC) $(flags) $(filename).c -o $(filename)

test: main
	./main 3

clean:
	ls --hide=Makefile --hide=main.c | xargs -d '\n' rm
