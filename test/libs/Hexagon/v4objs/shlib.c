int a = 10;
int b = 20;
int c = 30;
static int a1 = 40;
static int b1 = 50;
static int c1 = 60;

static int localfn() {
  return 0;
}

int (*hexrelativefn)() = &localfn;

int fn1() {
  printf("%d\n",b);
  printf("%d\n",b1);
  return 0;
}

int fn2() {
  printf("%d\n",c);
  printf("%d\n",c1);
  return 0;
}

int fn() {
  printf("hello world\n");
  printf("%d\n",a);
  printf("%d\n",a1);
  fn1();
  fn2();
  return 0;
}
