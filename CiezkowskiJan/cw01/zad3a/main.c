#include <time.h>
#include <ctype.h>
#include <sys/times.h>
#include "dll.h"

#define MAX_INPUT_SIZE 1000

int isnumber(char* argument, int len) {
  int i = 0;
  while (i < len) {
    if (isdigit(argument[i]) == 0) return 0;
    i++;
  }
  return 1;
}

struct TimeStructure
{
    clock_t real_time;
    clock_t user_time;
    clock_t system_time;
};

typedef struct TimeStructure TimeStructure;

TimeStructure get_time() {
    TimeStructure ts;
    struct tms ts_tms;
    ts.real_time = times(&ts_tms);
    ts.user_time = ts_tms.tms_utime;
    ts.system_time = ts_tms.tms_stime;
    return ts;
}

void print_times(TimeStructure* start, TimeStructure* end) {
    double real = (double)((end->real_time - start->real_time))/100;
    double user = (double)((end->user_time - start->user_time))/100;
    double system = (double)((end->system_time - start->system_time))/100;
    printf("Real time: %.2f s\n", real);
    printf("User time: %.2f s\n", user);
    printf("System time: %.2f s\n", system);
}

int main(void){
    load_lib("./lib_wc.so");
    int struct_initialized = 0;
    PointerArray structure;
    PointerArray* array;
    while(1){
        printf("REPL> ");
        char* input = calloc(MAX_INPUT_SIZE, sizeof(char));
        char* command = calloc(MAX_INPUT_SIZE, sizeof(char));
        char* argument = calloc(MAX_INPUT_SIZE, sizeof(char));
        char* temp;
        TimeStructure ts_start;
        TimeStructure ts_end;
        int command_length;
        temp = fgets(input, MAX_INPUT_SIZE, stdin);
        printf("wpisano: %s\n", temp);
        ts_start = get_time();
        char* separator = strchr(input, ' ');
        // obsluga komend bezargumentowych
        if (separator == NULL){
            separator = strchr(input, '\0');
            command_length = (int)(separator - input - 1);
            strncpy(command, input, command_length);
            if (strcmp(command, "destroy") == 0){
                if (struct_initialized){
                    free_memory(array);
                    struct_initialized = 0;
                }
                else fprintf(stderr, "structure not initialized\n");
            }
            else if (strcmp(command, "exit") == 0) {
                free(input);
                free(command);
                free(argument);
                break;
            }
            else fprintf(stderr, "invalid command\n");
        }
        // obsluga komend z argumentami
        else {
            command_length = (int)(separator - input);
            char* end = strchr(input + command_length + 1, '\0');
            int argument_length = (int)(end - separator - 2);
            strncpy(command, input, command_length);
            strncpy(argument, input + command_length + 1, argument_length);
            // init
            if (strcmp(command, "init") == 0) {
                if (struct_initialized == 0 && isnumber(argument, argument_length)) {
                    int size = atoi(argument);
                    if (size > 0) {
                    structure = create_array(size);
                    array = &structure;
                    struct_initialized = 1; 
                    }
                    else fprintf(stderr, "size cannot be negative");
                }
                else if (struct_initialized == 1) printf("structure already initialized\n");
                else printf("invalid argument\n");
            }
            else {
                if (struct_initialized == 1) {
                    // count
                    if (strcmp(command, "count") == 0) count(array, argument);
                    // show
                    else if (strcmp(command, "show") == 0) {
                        if (isnumber(argument, argument_length)){
                            int index = atoi(argument);
                            if (index >= array->current_size || index < 0) fprintf(stderr, "invalid index\n");
                            else printf("%s\n", get_content(array, atoi(argument)));
                        }
                        else fprintf (stderr, "argument must be a number\n");
                    }
                    // delete
                    else if (strcmp(command, "delete") == 0) {
                        if (isnumber(argument, argument_length)) remove_content(array, atoi(argument));
                        else fprintf(stderr, "argument must be a number\n");
                    }
                    else fprintf(stderr, "invalid command\n");
                }
                else fprintf(stderr, "structure not initialized\n");
                }
            }
        free(input);
        free(command);
        free(argument);
        ts_end = get_time();
        print_times(&ts_start, &ts_end);
    }
    close_lib();
}