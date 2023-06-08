#include "lib_wc.h"

PointerArray create_array(int size) {
  PointerArray new_array;
  new_array.array = calloc(size, sizeof(char *));
  new_array.max_size = size;
  new_array.current_size = 0;
  return new_array;
}

void count(PointerArray *array, const char *file_name) {
  if (array->current_size == array->max_size) {
    fprintf(stderr, "structure is full\n");
    return;
  }
  FILE *exists = fopen(file_name, "r");
  if (exists == NULL) {
    fprintf(stderr, "file not found\n");
    return;
  }
  char temp_file_name[] = "/tmp/wc_XXXXXX";
  mkstemp(temp_file_name);
  char command[100] = "wc -l -w -m ";
  strcat(command, file_name);
  strcat(command, " > ");
  strcat(command, temp_file_name);
  system(command);
  FILE *temp_file = fopen(temp_file_name, "r");
  fseek(temp_file, 0, SEEK_END);
  size_t size = ftell(temp_file);
  rewind(temp_file);
  char *file_content = calloc(size + 1, sizeof(char));

  fread(file_content, sizeof(char), size, temp_file);
  array->array[array->current_size] = file_content;
  fclose(temp_file);

  array->current_size++;

  remove(temp_file_name);
}

char *get_content(PointerArray *array, int index) {
  return array->array[index];
}

void remove_content(PointerArray *array, int index) {
  if (index >= array->current_size || index < 0) {
    fprintf(stderr, "invalid index\n");
    return;
  }
  free(array->array[index]);
  int i = index;
  while (i < array->current_size - 1) {
    array->array[i] = array->array[i + 1];
    i++;
  }
  array->array[i] = NULL;
  array->current_size--;
}

void free_memory(PointerArray *array) {
  for (int i = 0; i < array->current_size; i++) {
    free(array->array[i]);
  }
  free(array->array);
  array->current_size = 0;
}
