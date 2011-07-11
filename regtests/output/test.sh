#!/bin/bash

function setup {
	return 0;
}

function teardown {
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

	if [ ! -f ./func.exe ]; then
		return 1;
	fi
	return 0;
}

function testcase {
	make -f ./Makefile
	if [ ! -f ./func.ll ]; then
		return 1;
	fi
	return 0;
}
