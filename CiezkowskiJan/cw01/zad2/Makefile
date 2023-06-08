CC = gcc
flags = -Wall
soflags = -shared -o
filename = lib_wc
sources = main.c dll.h
path = $(shell pwd)

all: use_static use_shared use_dynamic

use_static: static_lib
	$(CC) $(flags) $(sources) $(filename).a -o main_static

use_shared: shared_lib
	$(CC) $(flags) $(sources) $(filename).so -o main_shared -Wl,-rpath=$(path)

use_dynamic: shared_lib
	$(CC) $(flags) $(sources) -D DYNAMIC -o main_dynamic -ldl

static_lib: $(filename).c
	$(CC) $(flags) -c $(filename).c
	ar rcs $(filename).a $(filename).o

shared_lib: $(filename).c
	$(CC) $(flags) -c -fPIC $(filename).c
	$(CC) $(flags) $(soflags) $(filename).so $(filename).o

clean:
	rm -rf *.o *.a *.so main_static main_shared main_dynamic
