MCLinker=MCLinker
all:
	@echo "--------------------";
	${MCLinker} -V -L/usr/lib -dB ./func.ll --start-group -Bdynamic -lm --end-group -o ./test.so 
	@echo "====================";
	${MCLinker} -dy -t -L/usr/lib -dB ./func.ll -lm -lz --start-group ./func.o --end-group -o ./test.so
	@echo "====================";
	${MCLinker} -dy -t -L/usr/lib -dB ./func.ll -lz ./func.o -lm -o ./test.so
	@echo "====================";
	${MCLinker} -dy -t -L/usr/lib -dB ./func.ll -\( ./func.o -\) -lm -o ./test.so
	@echo "====================";
	${MCLinker} -Bstatic -t -L/usr/lib -\( -dB ./func.ll ./func.o -\) -lm -o ./test.so
	${MCLinker} -dy -t -L/usr/lib -dB ./func.ll ./func.o -Bstatic -lm -o ./test.so
#	${MCLinker} -t ./test2.o ./test1.o -filetype=dso ./test3.o -march=arm -lc -lm -dB ./func.ll -o ./func.so -L.
#	${MCLinker} -relocation-model=pic -filetype=exe -march=arm ./func.ll -o ./func.exe
#	${MCLinker} -relocation-model=pic -filetype=arc -march=arm ./func.ll -o ./func.a
