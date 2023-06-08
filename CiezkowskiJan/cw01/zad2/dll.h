#include <dlfcn.h>

#include "lib_wc.h"

#ifdef DYNAMIC

void* lib = NULL;

int load_lib(const char* file_path) {
    lib = dlopen(file_path, RTLD_LAZY);
    
    if (lib == NULL) {
        fprintf(stderr, "something went wrong with the library");
        return 1;
    }

    *(void**) (&create_array) = dlsym(lib, "create_array");
    *(void**) (&count) = dlsym(lib, "count");
    *(void**) (&get_content) = dlsym(lib, "get_content");
    *(void**) (&remove_content) = dlsym(lib, "remove_content");
    *(void**) (&free_memory) = dlsym(lib, "free_memory");
    return 0;
}

void close_lib() {
    dlclose(lib);
}

#else

int load_lib(const char* file_path) {
    return(strcmp(file_path, ""));
}
void close_lib() {}

#endif