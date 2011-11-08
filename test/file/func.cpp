#include <func.h>

static int g_SV = 10;
int A::a = 10;

void f( int pArg1 )
{
	g_GV = pArg1;
}

int g( int pArg1 )
{
	return (g_SV+pArg1);
}

void A::f()
{
}

B::B()
{
}
