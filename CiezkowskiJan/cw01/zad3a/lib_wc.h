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

#ifdef DYNAMIC

    PointerArray (*create_array)(int);
    void (*count)(PointerArray*, const char*);
    char* (*get_content)(PointerArray*, int);
    void (*remove_content)(PointerArray*, int);
    void (*free_memory)(PointerArray*);

#else

    PointerArray create_array(int size);
    void count(PointerArray* array, const char* filename);
    char* get_content(PointerArray* array, int index);
    void remove_content(PointerArray* array, int index);
    void free_memory(PointerArray* array);

#endif
