; RUN: %MCLinker -mtriple=x86_64-pc-linux-gnu \
; RUN: -shared -soname=libgot1.so             \
; RUN: %p/got1.o -o %t.so

; RUN: diff -s %t.so %p/libgot1.golden.so
