/* g++ -c -m32 --function-sections -fPIC preemptible_function.cpp */
int a()
{
  return 1;
}

int b()
{
  return 1;
}

int foo()
{
  a();
  return 1;
}

int bar()
{
  b();
  return 1;
}
