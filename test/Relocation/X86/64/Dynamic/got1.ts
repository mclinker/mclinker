; RUN: %MCLinker -mtriple=x86_64-pc-linux-gnu                      \
; RUN: -filetype=dso -soname=libgot1.so                            \
; RUN: %p/got1.o -o %t.so

; RUN: diff -s %t.so %p/libgot1.golden.so
