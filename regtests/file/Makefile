MCLinker=MCLinker
all:
	clang++ -emit-llvm -S ./func.cpp -I. -o func.ll -ccc-host-triple arm-none-linux-gnueabi
	${MCLinker} -filetype=asm -march=arm -dB ./func.ll -o ./func.s
	${MCLinker} -filetype=obj -march=arm -dB ./func.ll -o ./func.o
	${MCLinker} -filetype=dso -march=arm -dB ./func.ll -o ./func.so
#	${MCLinker} -filetype=exe -march=arm ./func.ll -o ./func.exe
#	${MCLinker} -filetype=arc -march=arm ./func.ll -o ./func.a
