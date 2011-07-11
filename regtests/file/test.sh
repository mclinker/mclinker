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
	return 0;
}

function testcase {
	local RESULT=`file ./func.s | grep assembler`
	if [ -z "${RESULT}" ]; then
		echo "failed to cc1" > /dev/stderr
		return 1;
	fi
	local RESULT=`file ./func.o | grep relocatable`
	if [ -z "${RESULT}" ]; then
		echo "failed to as"
		return 1;
	fi
	local RESULT=`file ./func.so | grep shared`
	if [ -z "${RESULT}" ]; then
		echo "failed to ld -shared"
		return 1;
	fi
	local RESULT=`file ./func.exe | grep executable`
	if [ -z "${RESULT}" ]; then
		echo "failed to ld"
		return 1;
	fi
	return 0;
}
