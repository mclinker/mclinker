; RUN: cp %p/local_sym.o %t.o

; RUN: %MCLinker -mtriple="i386-linux-gnueabi" \
; RUN: -r %t.o -o %t.out.o && readelf -s %t.out.o | grep "1: 00000000     0 NOTYPE  LOCAL  DEFAULT    1 L1"
