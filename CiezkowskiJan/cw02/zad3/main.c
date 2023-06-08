#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#define MAX_PATH_LENGTH 1024

int main(void) {
    long long total_size = 0;
    char cwd[MAX_PATH_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "an error occured while using getcwd()");
        return 1;
    }
    DIR* directory = opendir(cwd);
    struct dirent* dirent = readdir(directory);
    struct stat stats;
    while (dirent != NULL) {
        stat(dirent->d_name, &stats);
        if(!S_ISDIR(stats.st_mode)) {
            printf("nazwa pliku: %s\nrozmiar pliku: %ld\n", dirent->d_name, stats.st_size);
            total_size += stats.st_size;
        }
        dirent = readdir(directory);
    }
    printf("total size of directory: %lld\n", total_size);
}