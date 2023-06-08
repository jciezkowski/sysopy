CC = gcc
flags = -Wall
filename = main
directory = $(shell pwd)/..

all: main

main: $(filename).c
	$(CC) $(flags) $(filename).c -o main

test: main
	./main $(directory)
	find $(directory) | xargs wc --bytes
clean:
	ls --hide=Makefile --hide=main.c | xargs -d '\n' rm