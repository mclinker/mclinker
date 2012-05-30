
#include <iostream>

using namespace std;

void f()
{
  throw 10;
}


int main()
{
  try {
    f();
  }
  catch (...) {
    cerr << "catch exception" << endl;
  }
  return 0;
}
