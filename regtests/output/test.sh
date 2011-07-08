#!/bin/bash

function test {
	make -f ./Makefile
	if [ ! -f ./func.ll ]; then
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
