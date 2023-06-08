CC = gcc
flags = -Wall
filename = main

all: main

main: main.c
	$(CC) $(flags) $(filename).c -o $(filename)

test: main
	./main ignore fork
	./main handler fork
	./main mask fork
	./main pending fork
	./main ignore exec
	./main pending exec
	./main mask exec

clean:
	ls --hide=Makefile --hide=main.c --hide=raport2.txt | xargs -d '\n' rm
