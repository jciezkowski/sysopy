CC = gcc
flags = -Wall
soflags = -shared -o
filename = lib_wc
sources = main.c dll.h
path = $(shell pwd)

all: test

use_static_O0: static_lib
	$(CC) $(flags) $(sources) $(filename).a -o main_static_O0 -O0

use_static_O: static_lib
	$(CC) $(flags) $(sources) $(filename).a -o main_static_O -O

use_static_Os: static_lib
	$(CC) $(flags) $(sources) $(filename).a -o main_static_Os -Os

use_shared_O0: shared_lib
	$(CC) $(flags) $(sources) $(filename).so -o main_shared_O0 -Wl,-rpath=$(path) -O0

use_shared_O: shared_lib
	$(CC) $(flags) $(sources) $(filename).so -o main_shared_O -Wl,-rpath=$(path) -O
	
use_shared_Os: shared_lib
	$(CC) $(flags) $(sources) $(filename).so -o main_shared_Os -Wl,-rpath=$(path) -Os

use_dynamic_O0: shared_lib
	$(CC) $(flags) $(sources) -D DYNAMIC -o main_dynamic_O0 -ldl -O0

use_dynamic_O: shared_lib
	$(CC) $(flags) $(sources) -D DYNAMIC -o main_dynamic_O -ldl -O

use_dynamic_Os: shared_lib
	$(CC) $(flags) $(sources) -D DYNAMIC -o main_dynamic_Os -ldl -Os

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
	echo show 7 >> commands.txt
	echo show 8 >> commands.txt
	echo show 9 >> commands.txt
	echo show 10 >> commands.txt
	echo show 11 >> commands.txt
	echo show 12 >> commands.txt
	echo show 13 >> commands.txt
	echo show 14 >> commands.txt
	echo show 15 >> commands.txt
	echo show 16 >> commands.txt
	echo show 17 >> commands.txt
	echo show 18 >> commands.txt
	echo delete 18 >> commands.txt
	echo delete 17 >> commands.txt
	echo delete 16 >> commands.txt
	echo delete 15 >> commands.txt
	echo delete 14 >> commands.txt
	echo delete 13 >> commands.txt
	echo delete 12 >> commands.txt
	echo delete 11 >> commands.txt
	echo delete 10 >> commands.txt
	echo delete 9 >> commands.txt
	echo delete 8 >> commands.txt
	echo delete 7 >> commands.txt
	echo delete 6 >> commands.txt
	echo delete 5 >> commands.txt
	echo delete 4 >> commands.txt
	echo delete 3 >> commands.txt
	echo delete 2 >> commands.txt
	echo delete 1 >> commands.txt
	echo delete 0 >> commands.txt
	echo destroy >> commands.txt
	echo exit >> commands.txt

test: use_static_O0 use_static_O use_static_Os use_shared_O0 use_shared_O use_shared_Os use_dynamic_O0 use_dynamic_O use_dynamic_Os initialize_test
	./main_static_O0 < commands.txt > results_static_O0.txt
	./main_static_O < commands.txt > results_static_O.txt
	./main_static_Os < commands.txt > results_static_Os.txt
	./main_shared_O0 < commands.txt > results_shared_O0.txt
	./main_shared_O < commands.txt > results_shared_O.txt
	./main_shared_Os < commands.txt > results_shared_Os.txt
	./main_dynamic_O0 < commands.txt > results_dynamic_O0.txt
	./main_dynamic_O < commands.txt > results_dynamic_O.txt
	./main_dynamic_Os < commands.txt > results_dynamic_Os.txt

static_lib: $(filename).c
	$(CC) $(flags) -c $(filename).c
	ar rcs $(filename).a $(filename).o

shared_lib: $(filename).c
	$(CC) $(flags) -c -fPIC $(filename).c
	$(CC) $(flags) $(soflags) $(filename).so $(filename).o

clean:
	rm -rf *.o *.a *.so main_static* main_shared* main_dynamic* commands.txt results_static*.txt results_shared*.txt results_dynamic*.txt
