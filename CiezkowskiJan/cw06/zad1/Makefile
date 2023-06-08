CC = gcc
flags = -Wall
filename = main

all: serwer klient

klient: klient.c
	$(CC) $(flags) klient.c -o klient

serwer: serwer.c
	$(CC) $(flags) serwer.c -o serwer

clean:
	ls --hide=Makefile --hide=klient.c --hide=serwer.c --hide=header.h --hide=komentarz_do_zadania.txt | xargs -d '\n' rm
