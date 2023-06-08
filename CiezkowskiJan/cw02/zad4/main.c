#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>

long long total = 0;

int read_directory(const char* path, const struct stat *stats, int flag) {
    if (flag == FTW_D) {
        return 0;
    }
    printf("name: %s\nsize: %ld\n", path, stats->st_size);
    total += stats->st_size;
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    if(ftw(argv[1], read_directory, 1) == -1) {
        fprintf(stderr, "could not open directory\n");
        return 2;
    }
    printf("total size: %lld\n", total);
    return 0;
}