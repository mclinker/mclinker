MCLinker=MCLinker
all:
	clang++ -emit-llvm -S ./func.cpp -I. -o func.ll -ccc-host-triple arm-none-linux-gnueabi
	${MCLinker} -relocation-model=pic -filetype=asm -march=arm -dB ./func.ll -o ./func.s
	${MCLinker} -relocation-model=pic -filetype=obj -march=arm -dB ./func.ll -o ./func.o
	${MCLinker} -relocation-model=pic -filetype=dso -march=arm -dB ./func.ll -o ./func.so
#	${MCLinker} -relocation-model=pic -filetype=exe -march=arm ./func.ll -o ./func.exe
#	${MCLinker} -relocation-model=pic -filetype=arc -march=arm ./func.ll -o ./func.a
