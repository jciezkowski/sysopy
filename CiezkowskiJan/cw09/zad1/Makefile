CC = gcc
flags = -Wall -pthread
filename = main

all: main

main: main.c
	$(CC) $(flags) $(filename).c -o $(filename) -lpthread

test: main
	./main > results.txt

clean:
	ls --hide=Makefile --hide=main.c --hide=results.txt | xargs -d '\n' rm
