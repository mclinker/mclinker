MCLinker=MCLinker
all:
	clang++ -emit-llvm -S ./func.cpp -I. -o func.ll -ccc-host-triple arm-none-linux-gnueabi
	${MCLinker} -dy -t -L/usr/lib -dB ./func.ll ./func.o -lm -o ./test.so
#	${MCLinker} -t ./test2.o ./test1.o -filetype=dso ./test3.o -march=arm -lc -lm -dB ./func.ll -o ./func.so -L.
#	${MCLinker} -relocation-model=pic -filetype=exe -march=arm ./func.ll -o ./func.exe
#	${MCLinker} -relocation-model=pic -filetype=arc -march=arm ./func.ll -o ./func.a
