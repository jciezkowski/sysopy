CC = gcc
flags = -Wall
filename = main
directory = $(shell pwd)

all: main

main: main.c
	$(CC) $(flags) $(filename).c -o $(filename)

test: main
	./main $(directory)/empty_dir
	./main $(directory)
	./main $(directory)/..

clean:
	ls --hide=Makefile --hide=main.c --hide=empty_dir | xargs -d '\n' rm
