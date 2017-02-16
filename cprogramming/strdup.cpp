#include <cstring>
#include <stdio.h>
#include <string>
#include <stdlib.h>

int main() {
  std::string s("abcdefg");
  char *ps = strdup(s.c_str());
  printf("%s\n", ps);
  free(ps);
}
