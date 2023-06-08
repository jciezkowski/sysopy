#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_INPUT_SIZE 1000


#ifdef USE_LIBRARY
    void convert_file(char to_find, char to_replace, const char* in_file, const char* out_file) {
        FILE* in = fopen(in_file, "r");
        FILE* out = fopen(out_file, "w");
        if (in == NULL || out == NULL){
            fprintf(stderr, "failed to open the file\n");
            return;
        }
        char character;
        while (fread(&character, sizeof(char), 1, in) == 1) {
            if(character == to_find) fwrite(&to_replace, sizeof(char), 1, out);
            else fwrite(&character, sizeof(char), 1, out);
        }
        fclose(in);
        fclose(out);
    }
#else
    void convert_file(char to_find, char to_replace, const char* in_file, const char* out_file) {
        char* path1 = calloc(MAX_INPUT_SIZE, sizeof(char));
        char* path2 = calloc(MAX_INPUT_SIZE, sizeof(char));
        strcat(path1, "./");
        strcat(path2, "./");
        int in = open(strcat(path1, in_file), O_RDONLY);
        int out = open(strcat(path2, out_file), O_WRONLY | O_CREAT, 777);
        if(in < 0 || out < 0) {
            printf("%s\n", path1);
            printf("%s\n", path2);
            fprintf(stderr, "failed to open the file\n");
            free(path1);
            free(path2);
            return;
        }
        char character;
        while (read(in, &character, 1) == 1) {
            if(character == to_find) write(out, &to_replace, 1);
            else write(out, &character, 1);
        }
        free(path1);
        free(path2);
        close(in);
        close(out);
    }

#endif

void initialize_conversion(char** argv) {
    char to_find;
    char to_replace;
    // sprawdzenie poprawności wprowadzanych danych
    if (strlen(argv[1]) != 1 || strlen(argv[2]) != 1) {
        fprintf(stderr, "provided arguments are not singular characters\n");
        return;
    }
    if (argv[1] == argv[2]) {
        fprintf(stderr, "provided two identical characters\n");
        return;
    }
    to_find = argv[1][0];
    to_replace = argv[2][0];
    char* in_file = argv[3];
    char* out_file = argv[4];
    convert_file(to_find, to_replace, in_file, out_file);
}

clock_t get_time() {
    struct tms ts_tms;
    clock_t current_time = times(&ts_tms);
    return current_time;
}

int main(int argc, char** argv) {
    clock_t start, end;
    if (argc != 5) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    start = get_time();
    initialize_conversion(argv);
    end = get_time();
    printf("czas operacji: %d ticks\n", (int)(end - start));
}