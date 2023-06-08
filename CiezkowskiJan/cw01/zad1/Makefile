CC = gcc
flags = -Wall
soflags = -shared -o
filename = lib_wc

all: shared_lib static_lib

shared_lib: $(filename).c
	$(CC) $(flags) -c -fPIC $(filename).c
	$(CC) $(flags) $(soflags) $(filename).so $(filename).o

static_lib: $(filename).c
	$(CC) $(flags) -c $(filename).c
	ar rcs $(filename).a $(filename).o

clean:
	rm -rf *.o *.a *.so
