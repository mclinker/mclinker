; RUN: %MCLinker -march=x86 -pie                 \
; RUN: -dynamic-linker /lib/ld-linux.so.2        \
; RUN: %p/../../libs/X86/Linux/crt1.o            \
; RUN: %p/../../libs/X86/Linux/crti.o            \
; RUN: %p/../../libs/X86/Linux/crtbegin.o        \
; RUN: %p/virtual_function.o                     \
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

; foo and Derived::bar should be folded into the same one
; RUN: readelf -Ds %t.out | FileCheck %s
; CHECK: 000009e3    10 FUNC    GLOBAL DEFAULT  10 _Z3foov
; CHECK: 000009e3    10 FUNC    WEAK   DEFAULT  10 _ZN7Derived3barEv
