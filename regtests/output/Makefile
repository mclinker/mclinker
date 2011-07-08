MCLinker=MCLinker
all:
	clang++ -emit-llvm -S ./func.cpp -I. -o func.ll -ccc-host-triple arm-none-linux-gnueabi
	${MCLinker} -relocation-model=pic -filetype=dso -march=arm ./func.ll -o ./func.so
	${MCLinker} -relocation-model=pic -filetype=exe -march=arm ./func.ll -o ./func.exe
