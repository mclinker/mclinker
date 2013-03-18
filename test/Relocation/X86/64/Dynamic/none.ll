; RUN: %MCLinker -mtriple=x86_64-pc-linux-gnu                      \
; RUN: -filetype=dso                                               \
; RUN: %p/none.o -o %t.so

; RUN: readelf -sD %t.so | FileCheck %s -check-prefix=SYM
; SYM: FUNC{{[ ]+}}GLOBAL{{[ ]+}}DEFAULT{{[ ]+}}{{[0-9]+}}{{[ ]+}}foo
