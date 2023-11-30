#include "utils.h"
#include <cstdio>
namespace WE::Utils {

std::string ReadFile(const char *path) {
  long file_length;
  FILE *file = fopen(path, "rb");

  if (file) {
    fseek(file, 0, SEEK_END);
    file_length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char file_content[file_length + 1];
    size_t size = fread(file_content, 1, file_length, file);
    if (size == 0)
      printf("file is empty!\n");
    fclose(file);
    file_content[file_length] = '\0';
    return file_content;
  }
  printf("Failed to open file: %s!\n", path);
  return "";
}

} // namespace WE::Utils
