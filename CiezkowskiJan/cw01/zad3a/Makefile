CC = gcc
flags = -Wall
soflags = -shared -o
filename = lib_wc
sources = main.c dll.h
path = $(shell pwd)

all: test

libraries: use_static use_shared use_dynamic

use_static: static_lib
	$(CC) $(flags) $(sources) $(filename).a -o main_static

use_shared: shared_lib
	$(CC) $(flags) $(sources) $(filename).so -o main_shared -Wl,-rpath=$(path)

use_dynamic: shared_lib
	$(CC) $(flags) $(sources) -D DYNAMIC -o main_dynamic -ldl

initialize_test:
	echo init 100 > commands.txt
	ls -p | grep -v / | sed 's/^/count /' >> commands.txt
	echo show 0 >> commands.txt
	echo show 1 >> commands.txt
	echo show 2 >> commands.txt
	echo show 3 >> commands.txt
	echo show 4 >> commands.txt
	echo show 5 >> commands.txt
	echo show 6 >> commands.txt
	echo delete 6 >> commands.txt
	echo delete 5 >> commands.txt
	echo delete 4 >> commands.txt
	echo delete 3 >> commands.txt
	echo delete 2 >> commands.txt
	echo delete 1 >> commands.txt
	echo delete 0 >> commands.txt
	echo destroy >> commands.txt
	echo exit >> commands.txt

test: use_static use_shared use_dynamic initialize_test
	./main_static < commands.txt > results_static.txt
	./main_shared < commands.txt > results_shared.txt
	./main_dynamic < commands.txt > results_dynamic.txt

static_lib: $(filename).c
	$(CC) $(flags) -c $(filename).c
	ar rcs $(filename).a $(filename).o

shared_lib: $(filename).c
	$(CC) $(flags) -c -fPIC $(filename).c
	$(CC) $(flags) $(soflags) $(filename).so $(filename).o

clean:
	rm -rf *.o *.a *.so commands.txt results_static*.txt results_shared*.txt results_dynamic*.txt main_static main_shared main_dynamic
