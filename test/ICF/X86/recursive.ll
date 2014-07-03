; RUN: %MCLinker -march=x86                      \
; RUN: -dynamic-linker /lib/ld-linux.so.2        \
; RUN: %p/../../libs/X86/Linux/crt1.o            \
; RUN: %p/../../libs/X86/Linux/crti.o            \
; RUN: %p/../../libs/X86/Linux/crtbegin.o        \
; RUN: %p/recursive.o                            \
; RUN: %p/../../libs/X86/Linux/crtend.o          \
; RUN: %p/../../libs/X86/Linux/crtn.o            \
; RUN: %p/../../libs/X86/Linux/libc_nonshared.a  \
; RUN: --as-needed                               \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2     \
; RUN: --no-as-needed                            \
; RUN: %p/../../libs/X86/Linux/libc.so.6         \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2     \
; RUN: --icf=all -o %t.out

; foo and bar should be folded into the same one
; RUN: readelf -Ds %t.out | FileCheck %s
; CHECK: 0804858d    23 FUNC    GLOBAL DEFAULT  10 _Z3barv
; CHECK: 0804858d    23 FUNC    GLOBAL DEFAULT  10 _Z3foov
