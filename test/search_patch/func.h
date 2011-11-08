#ifndef FUNC_H
#define FUNC_H

int g_GV = 10;

void f( int pArg1 ) __attribute__((destructor));
int g( int pArg1 ) __attribute__((constructor));

class A
{
public:
	A( int p ) { a = p; }
	~A() { a = 0; }

	A& operator=( int p ) {
		a=p;
	}

	virtual void f();
private:
	static int a;
};


class B
{
public:
	static B* self() {
		static B b;
		return &b;
	}

	static A* a_out() {
		static A a(10);
		return &a;
	}
private:
	B();
};
#endif
