CC = gcc
flags = -Wall
filename = main

all: main

main: main.c
	$(CC) $(flags) $(filename).c -o $(filename)

test: main
	./main 0.000000001 1000

clean:
	ls --hide=Makefile --hide=main.c --hide=raport.txt | xargs -d '\n' rm
