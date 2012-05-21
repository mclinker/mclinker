
#include <iostream>

using namespace std;

extern int x;
void f()
{
  if (0 == x)
    throw 10;
}


void g()
{
  try {
    f();
  }
  catch (...) {
    cerr << "catch exception" << endl;
  }
}
