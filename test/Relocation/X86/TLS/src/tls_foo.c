__thread int tls1 = 1;
__thread int tls2 = 2;
__thread int tls_nodef;
static __thread int static_tls_nodef;
static __thread int static_tls = 5;

int foo()
{
  tls_nodef = 3;
  static_tls_nodef = 4;
  return static_tls + static_tls_nodef + tls1 + tls2 + tls_nodef;
}
