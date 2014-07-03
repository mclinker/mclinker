; RUN: %MCLinker -march=x86                      \
; RUN: -dynamic-linker /lib/ld-linux.so.2        \
; RUN: %p/../../libs/X86/Linux/crt1.o            \
; RUN: %p/../../libs/X86/Linux/crti.o            \
; RUN: %p/../../libs/X86/Linux/crtbegin.o        \
; RUN: %p/safe_icf_exec.o                        \
; RUN: %p/../../libs/X86/Linux/crtend.o          \
; RUN: %p/../../libs/X86/Linux/crtn.o            \
; RUN: %p/../../libs/X86/Linux/libc_nonshared.a  \
; RUN: --as-needed                               \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2     \
; RUN: --no-as-needed                            \
; RUN: %p/../../libs/X86/Linux/libc.so.6         \
; RUN: %p/../../libs/X86/Linux/libstdc++.so.6    \
; RUN: %p/../../libs/X86/Linux/libgcc_s.so.1     \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2     \
; RUN: --icf=safe -o %t.out

; f1, f2, and f3 should not be folded into the same one, but ctors/dtors would.
; RUN: readelf -Ds %t.out | FileCheck %s
; CHECK: 080486a4    11 FUNC    GLOBAL DEFAULT  10 _Z2f2i
; CHECK: 08048699    11 FUNC    GLOBAL DEFAULT  10 _Z2f1i
; CHECK: 08048694     5 FUNC    WEAK   DEFAULT  10 _ZN3FooC2Ev
; CHECK: 08048694     5 FUNC    WEAK   DEFAULT  10 _ZN3FooD2Ev
; CHECK: 080486af    11 FUNC    GLOBAL DEFAULT  10 _Z2f3i
