#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#ifndef READ_BLOCK
    #define READ_BLOCK 1
#endif

void reverse(char* block) {
    int left = 0;
    int right = strlen(block) - 1;
    char temp;
    while (left < right) {
        temp = block[left];
        block[left] = block[right];
        block[right] = temp;
        left += 1;
        right -= 1;
    }
}

void read_write(const char* in_file, const char* out_file) {
    FILE* in = fopen(in_file, "r");
    FILE* out = fopen(out_file, "w");
    if (in == NULL || out == NULL) {
        fprintf(stderr, "failed to open the file\n");
        return;
    }
    char character[READ_BLOCK];
    fseek(in, -READ_BLOCK, SEEK_END);
    int position = (int)ftell(in);
    while (position >= 0) {
        fread(character, sizeof(char), READ_BLOCK, in);
        reverse(character);
        fwrite(character, sizeof(char), READ_BLOCK, out);
        position -= READ_BLOCK;
        fseek(in, -2 * READ_BLOCK, SEEK_CUR);
    }
    fseek(in, 0, SEEK_SET);
    int chars_left = READ_BLOCK + position;
    if (chars_left > 0) {
        char last_block[chars_left];
        fread(last_block, sizeof(char), chars_left, in);
        reverse(last_block);
        fwrite(last_block, sizeof(char), chars_left, out);
    }
    fclose(in);
    fclose(out);
}

clock_t get_time() {
    struct tms ts_tms;
    clock_t current_time = times(&ts_tms);
    return current_time;
}

int main(int argc, char** argv) {
    clock_t start, end;
    if (argc != 3) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }
    start = get_time();
    read_write(argv[1], argv[2]);
    end = get_time();
    printf("czas operacji: %d ticks\n", (int)(end - start));
}