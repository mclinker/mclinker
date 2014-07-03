; RUN: %MCLinker -march=x86 -shared              \
; RUN: -dynamic-linker /lib/ld-linux.so.2        \
; RUN: %p/../../libs/X86/Linux/crti.o            \
; RUN: %p/../../libs/X86/Linux/crtbegin.o        \
; RUN: %p/preemptible_function.o                 \
; RUN: %p/../../libs/X86/Linux/crtend.o          \
; RUN: %p/../../libs/X86/Linux/crtn.o            \
; RUN: %p/../../libs/X86/Linux/libc_nonshared.a  \
; RUN: --as-needed                               \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2     \
; RUN: --no-as-needed                            \
; RUN: %p/../../libs/X86/Linux/libc.so.6         \
; RUN: %p/../../libs/X86/Linux/libstdc++.so.6    \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2     \
; RUN: --icf=all -o %t.out

; foo and bar should not be folded into the same one
; RUN: readelf -Ds %t.out | FileCheck %s
; CHECK: 0000046f    34 FUNC    GLOBAL DEFAULT   9 _Z3barv
; CHECK: 0000044d    34 FUNC    GLOBAL DEFAULT   9 _Z3foov
; CHECK: 00000443    10 FUNC    GLOBAL DEFAULT   9 _Z1bv
; CHECK: 00000443    10 FUNC    GLOBAL DEFAULT   9 _Z1av
