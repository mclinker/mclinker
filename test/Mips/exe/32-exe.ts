; RUN: %MCLinker -mtriple=mipsel-linux-gnueabi -filetype=exe \
; RUN: %p/../../libs/MIPS/Linux/32/crt1.o \
; RUN: %p/../../libs/MIPS/Linux/32/crti.o \
; RUN: %p/../../libs/MIPS/Linux/32/crtbegin.o \
; RUN: %p/main32.o %p/bar32.o \
; RUN: %p/../../libs/MIPS/Linux/32/libc_nonshared.a \
; RUN: --as-needed \
; RUN: %p/../../libs/MIPS/Linux/32/ld.so.1 \
; RUN: --no-as-needed \
; RUN: %p/../../libs/MIPS/Linux/32/crtend.o \
; RUN: %p/../../libs/MIPS/Linux/32/crtn.o \
; RUN: %p/../../libs/MIPS/Linux/32/libc.so.6 \
; RUN: -o %t.exe

; RUN: diff -s %t.exe %p/a32.golden.out
