; RUN: %MCLinker -z relro -shared -mtriple=x86-linux-gnu            \
; RUN: --dynamic-linker=/lib/ld-linux.so.2                          \
; RUN: -march=x86 %p/../libs/X86/Linux/crt1.o                       \
; RUN: %p/../libs/X86/Linux/crti.o %p/../libs/X86/Linux/crtbegin.o  \
; RUN: %p/nested_ar/empty.a                                         \
; RUN: %p/../libs/X86/Linux/crtend.o                                \
; RUN: %p/../libs/X86/Linux/crtn.o %p/../libs/X86/Linux/libc.so.6   \
; RUN: %p/../libs/X86/Linux/libc_nonshared.a                        \
; RUN: --as-needed                                                  \
; RUN: %p/../libs/X86/Linux/ld-linux.so.2                           \
; RUN: --no-as-needed                                               \
; RUN: %p/../libs/X86/Linux/libgcc_s.so.1                           \
; RUN: %p/../libs/X86/Linux/libm.so.6                               \
; RUN: %p/../libs/X86/Linux/libstdc++.so.6 -o %t.out
