/* g++ -c -m32 --function-sections safe_icf_exec.cpp */
#include <cstdio>
class Foo
{
public:
  Foo() { }
  ~Foo() { }
};

int f1(int arg)
{
  return arg + 1;
}

int f2(int arg)
{
  return arg + 1;
}

int f3(int arg)
{
  return arg + 1;
}

int main()
{
  Foo foo;
  int (*f)(int) = f2;
  printf("%p\t%d\n", f1, f1(1));
  printf("%p\t%d\n", f2, f2(1));
  printf("%p\t%d\n", f3, f3(1));
  return 0;
}
