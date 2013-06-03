static int bar1(void)
{
  return 1;
}
static int bar2(void)
{
  return bar1() + 2;
}
int bar(void)
{
  return bar2();
}
