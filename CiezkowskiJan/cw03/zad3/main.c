#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/limits.h>

void browse(char* path, const char* to_find, int* length) {
    DIR* dir = opendir(path);
    struct dirent* dirent;
    struct stat stats;
    char* file_path = calloc(PATH_MAX, sizeof(char));
    char* file_content = calloc(*length + 1, sizeof(char));

    FILE* file_ptr;
    pid_t pid;
    while ((dirent = readdir(dir))) {
        if(strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..")) {
            strcpy(file_path, path);
            strcat(file_path, "/");
            strcat(file_path, dirent->d_name);
            stat(file_path, &stats);
            if(S_ISDIR(stats.st_mode)) {
                pid = fork();
                if (pid == 0) {
                    browse(file_path, to_find, length);
                    exit(EXIT_SUCCESS);
                }
            }
            else {
                file_ptr = fopen(file_path, "r");
                fread(file_content, sizeof(char), *length, file_ptr);
                if (strcmp(to_find, file_content) == 0) {
                    printf("path: %s\npid: %d\n", file_path, (int)getpid());
                }
                fclose(file_ptr);
            }
        }
    }
    free(file_content);
    free(file_path);
    closedir(dir);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    int length = strlen(argv[2]);
    if (length > 255) {
        fprintf(stderr, "invalid length\n");
        return 2;
    }
    browse(argv[1], argv[2], &length);
    return 0;
}