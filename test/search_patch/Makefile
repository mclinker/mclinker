MCLinker=MCLinker
TESTLDFLAGS=-lm
all:
	clang++ -emit-llvm -S ./func.cpp -I. -o func.ll -ccc-host-triple arm-none-linux-gnueabi
	${MCLinker} -t ${TESTLDFLAGS} -filetype=dso -march=arm ./func.ll -o ./func.so > result
#	${MCLinker} -filetype=exe -march=arm ./func.ll -o ./func.exe
#	${MCLinker} -filetype=arc -march=arm ./func.ll -o ./func.a
