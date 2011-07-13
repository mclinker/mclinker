MCLinker=MCLinker
TESTLDFLAGS=-lm
all:
	clang++ -emit-llvm -S ./func.cpp -I. -o func.ll -ccc-host-triple arm-none-linux-gnueabi
	${MCLinker} -t ${TESTLDFLAGS} -relocation-model=pic -filetype=dso -march=arm ./func.ll -o ./func.so > result
#	${MCLinker} -relocation-model=pic -filetype=exe -march=arm ./func.ll -o ./func.exe
#	${MCLinker} -relocation-model=pic -filetype=arc -march=arm ./func.ll -o ./func.a
