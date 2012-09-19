#include <stdio.h>

extern __thread int tls_nodef;
extern __thread int tls_def;

int foo()
{
  tls_nodef = 2;
  return (tls_nodef + tls_def);
}
