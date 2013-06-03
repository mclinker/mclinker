#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int puts(const char *);
extern char **environ;

int bar(void);

static void foo()
{
  puts("Hello, World!");
}

int main()
{
  struct stat sb;
  foo();
  puts(*environ);
  stat("a.out", &sb);
  puts("Bye, bye.");
  return bar() - 3;
}
