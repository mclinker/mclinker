; RUN: %MCLinker -mtriple=x86_64-pc-linux-gnu -shared -soname=libplt1.so \
; RUN: %p/plt1.o -o %t.so

; RUN: diff -s %t.so %p/libplt1.golden.so
