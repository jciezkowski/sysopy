CC = gcc
flags = -Wall
filename = main

all: serwer klient

klient: klient.c
	$(CC) $(flags) klient.c -o klient -lpthread

serwer: serwer.c
	$(CC) $(flags) serwer.c -o serwer -lpthread

clean:
	ls --hide=Makefile --hide=klient.c --hide=serwer.c --hide=header.h --hide=*.txt | xargs -d '\n' rm
