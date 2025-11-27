#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *file_name) {
  // open file.
  FILE *file_ptr = fopen(file_name, "rb");
  if (!file_ptr) {
    return NULL;
  }
  // seek to end to get file size.
  if (fseek(file_ptr, 0, SEEK_END) != 0) {
    fclose(file_ptr);
    return NULL;
  }
  long file_size = ftell(file_ptr);
  // check if file size is correct.
  if (file_size < 0) {
    fclose(file_ptr);
    printf("File too big :(\n");
    return NULL;
  }
  rewind(file_ptr);
  // allocate memory for the buffer.
  char *buffer = (char *)malloc(file_size + 1);
  if (!buffer) {
    fclose(file_ptr);
    return NULL;
  }
  // get the file into buffer.
  size_t file_length = fread(buffer, 1, (size_t)file_size, file_ptr);
  fclose(file_ptr);
  buffer[file_length] = '\0';
  return buffer;
}