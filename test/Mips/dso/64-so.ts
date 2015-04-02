; RUN: %MCLinker -mtriple=mips64el-linux-gnueabi -filetype=dso -shared \
; RUN: %p/../../libs/MIPS/Linux/64/crti.o \
; RUN: %p/../../libs/MIPS/Linux/64/crtbeginS.o \
; RUN: %p/foo64.o \
; RUN: %p/../../libs/MIPS/Linux/64/libc_nonshared.a \
; RUN: --as-needed \
; RUN: %p/../../libs/MIPS/Linux/64/ld.so.1 \
; RUN: --no-as-needed \
; RUN: %p/../../libs/MIPS/Linux/64/crtendS.o \
; RUN: %p/../../libs/MIPS/Linux/64/crtn.o \
; RUN: %p/../../libs/MIPS/Linux/64/libc.so.6 \
; RUN: -soname libfoo.so -o %t.so

; RUN: diff -s %t.so %p/libfoo64.golden.so
