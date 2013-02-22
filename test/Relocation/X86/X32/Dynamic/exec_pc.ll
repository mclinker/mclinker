; RUN: %MCLinker -mtriple=x86_64-pc-linux-gnux32                    \
; RUN: -dynamic-linker /libx32/ld-linux-x32.so.2                    \
; RUN: %p/../../../../libs/X86/Linux/X32/crt1.o                     \
; RUN: %p/../../../../libs/X86/Linux/X32/crti.o                     \
; RUN: %p/exec_pc.o                                                 \
; RUN: %p/../../../../libs/X86/Linux/X32/libc_nonshared.a           \
; RUN: --as-needed                                                  \
; RUN: %p/../../../../libs/X86/Linux/X32/ld-linux-x32.so.2          \
; RUN: %p/../../../../libs/X86/Linux/X32/crtn.o                     \
; RUN: %p/../../../../libs/X86/Linux/X32/libc.so.6 -o %t.exe

; RUN: readelf -r %t.exe | FileCheck %s -check-prefix=REL
; REL: R_X86_64_JUMP_SLO{{[ ]+}}{{[0]+}} puts + 0
