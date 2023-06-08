CC = gcc
flags = -Wall -pthread
filename = main

all: main

main: main.c
	$(CC) $(flags) $(filename).c -o $(filename) -lrt

clean:
	ls --hide=Makefile --hide=main.c | xargs -d '\n' rm
