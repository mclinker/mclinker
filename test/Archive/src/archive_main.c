#include <stdio.h>

extern int archive_test1();

int main() {
  printf("%d\n", archive_test1());
  return 0;
}
