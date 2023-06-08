#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define COMMAND_LENGTH 1024


int main(int argc, char** argv) {
    if (argc != 2 && argc != 4) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    FILE* file_ptr;
    if (argc == 2) {
        if (strcmp(argv[1], "nadawca") == 0) {
            file_ptr = popen("mail -H | sort -k 3", "r");
        }
        else if (strcmp(argv[1], "data") == 0) {
            file_ptr = popen("mail -H | sort -k 4", "r");
        }
        else {
            fprintf(stderr, "invalid argument\n");
            return 2;
        }
        char character;
        while (fread(&character, sizeof(char), 1, file_ptr)) printf("%c", character);
    }
    else if (argc == 4) {
        char command[COMMAND_LENGTH];
        sprintf(command, "mail -s %s %s", argv[2], argv[1]);
        file_ptr = popen(command, "w");
        fputs("\n", file_ptr);
        fputs(argv[3], file_ptr);
        fputs("\n", file_ptr);
    }
    pclose(file_ptr);
}