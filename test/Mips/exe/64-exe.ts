; RUN: %MCLinker -mtriple=mips64el-linux-gnueabi \
; RUN: %p/../../libs/MIPS/Linux/64/crt1.o \
; RUN: %p/../../libs/MIPS/Linux/64/crti.o \
; RUN: %p/../../libs/MIPS/Linux/64/crtbegin.o \
; RUN: %p/main64.o %p/bar64.o \
; RUN: %p/../../libs/MIPS/Linux/64/libc_nonshared.a \
; RUN: --as-needed \
; RUN: %p/../../libs/MIPS/Linux/64/ld.so.1 \
; RUN: --no-as-needed \
; RUN: %p/../../libs/MIPS/Linux/64/crtend.o \
; RUN: %p/../../libs/MIPS/Linux/64/crtn.o \
; RUN: %p/../../libs/MIPS/Linux/64/libc.so.6 \
; RUN: -o %t.exe

; RUN: diff -s %t.exe %p/a64.golden.out
