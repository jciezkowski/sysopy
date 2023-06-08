CC = gcc
flags = -Wall

all: catcher sender

catcher: catcher.c
	$(CC) $(flags) catcher.c -o catcher

sender: sender.c
	$(CC) $(flags) sender.c -o sender

clean:
	ls --hide=Makefile --hide=sender.c --hide=catcher.c | xargs -d '\n' rm
