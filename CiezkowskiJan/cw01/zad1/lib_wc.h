#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lib_wc
{
    char** array;
    int max_size;
    int current_size;
};

typedef struct lib_wc PointerArray;

PointerArray create_array(int size);
void count(PointerArray* array, const char* filename);
char* get_content(PointerArray* array, int index);
void remove_content(PointerArray* array, int index);
void free_memory(PointerArray* array);
