/* g++ -c -m32 --function-sections -fPIC virtual_function.cpp */
#include <cstdio>
int foo()
{
  return 123;
}

class Base
{
public:
  virtual ~Base()
  { }
  virtual int bar()
  {
    return 456;
  }
};

class Derived : public Base
{
public:
  virtual int bar()
  {
    return 123;
  }
};

int main()
{
  Derived* d = new Derived();
  Base* b = d;
  printf("%d\n", foo());
  printf("%d\n", b->bar());
  return 0;
}
