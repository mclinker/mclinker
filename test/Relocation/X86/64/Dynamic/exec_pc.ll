; RUN: %MCLinker -mtriple=x86_64-pc-linux-gnu                      \
; RUN: --dynamic-linker=/lib64/ld-linux-x86-64.so.2                \
; RUN: %p/../../../../libs/X86/Linux/64/crt1.o                     \
; RUN: %p/../../../../libs/X86/Linux/64/crti.o                     \
; RUN: %p/exec_pc.o                                                \
; RUN: %p/../../../../libs/X86/Linux/64/libc_nonshared.a           \
; RUN: --as-needed                                                 \
; RUN: %p/../../../../libs/X86/Linux/64/ld-linux-x86-64.so.2       \
; RUN: %p/../../../../libs/X86/Linux/64/crtn.o                     \
; RUN: %p/../../../../libs/X86/Linux/64/libc.so.6 -o %t.exe

; RUN: readelf -r %t.exe | FileCheck %s -check-prefix=REL
; REL: R_X86_64_JUMP_SLO{{[ ]+}}{{[0]+}} puts + 0
