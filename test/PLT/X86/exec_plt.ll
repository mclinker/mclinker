; RUN: %MCLinker -z relro -march=x86 -mtriple=x86-linux-gnu \
; RUN: %p/../../libs/X86/Linux/crt1.o %p/../../libs/X86/Linux/crti.o \
; RUN: %p/main.o %p/../../libs/X86/Linux/libc_nonshared.a \
; RUN: %p/../../libs/X86/Linux/crtn.o \
; RUN: %p/../../libs/X86/Linux/libc.so.6 -o %t.out

; RUN: readelf -S %t.out | grep .plt | FileCheck %s -check-prefix=SECTION
; SECTION: .rel.plt
; SECTION-NEXT: .plt
; SECTION-NEXT: .got.plt
