/* g++ -c -m32 --function-sections recursive.cpp */
#include <cstdio>

int test() {
  return 0;
}

int foo() {
  test();
  foo();
  return 1;
}

int bar() {
  test();
  bar();
  return 1;
}

int main() {
  printf("foo: %p\n", foo);
  printf("bar: %p\n", bar);
  return 1;
}
