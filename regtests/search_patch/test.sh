#!/bin/bash

function setup {
	make -f ./Makefile
	if [ ! -f ./func.ll ]; then
		return 1;
	fi
	if [ ! -f ./func.s ]; then
		return 1;
	fi
	if [ ! -f ./func.o ]; then
		return 1;
	fi
	if [ ! -f ./func.a ]; then
		return 1;
	fi
	if [ ! -f ./func.so ]; then
		return 1;
	fi
	return 0;
}

function teardown {
	if [ -f ./func.ll ]; then
		rm ./func.ll;
	fi
	if [ -f ./func.s ]; then
		rm ./func.s;
	fi
	if [ -f ./func.o ]; then
		rm ./func.o;
	fi
	if [ -f ./func.a ]; then
		rm ./func.a;
	fi
	if [ -f ./func.so ]; then
		rm ./func.so;
	fi

	if [ -f ./func.exe ]; then
		rm ./func.exe;
	fi

	if [ -f ./result ]; then
		rm ./result;
	fi
	return 0;
}

function testcase {
	LIBC=`cat ./result | grep -lm`
	if [ -z "${LIBC}" ]; then
		echo "failed to search -lm" > /dev/stderr
		return 1;
	fi
	return 0;
}
