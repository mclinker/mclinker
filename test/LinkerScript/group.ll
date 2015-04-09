; create a fake libc.so to test searching through library search path
; RUN: echo "/* GNU ld script                                            \
; RUN:       Use the shared library, but some functions are only in      \
; RUN:       the static library, so try that secondarily.  */            \
; RUN:       OUTPUT_FORMAT(elf32-i386)                                   \
; RUN:       GROUP ( libc.so.6                                           \
; RUN:               libc_nonshared.a                                    \
; RUN:               AS_NEEDED ( ld-linux.so.2 ) )"                      \
; RUN: > %t.libc.so

; check using -L in GROUP content resolving
; RUN: %MCLinker --eh-frame-hdr -z relro -o %t.out \
; RUN: -mtriple=x86-linux-gnu -march=x86           \
; RUN: --dynamic-linker=/lib/ld-linux.so.2         \
; RUN: -L%p/../libs/X86/Linux                      \
; RUN: %p/../libs/X86/Linux/crt1.o                 \
; RUN: %p/../libs/X86/Linux/crti.o                 \
; RUN: %p/../libs/X86/Linux/crtbegin.o             \
; RUN: %p/../Exceptions/Itanium/exception_main.o   \
; RUN: %p/../libs/X86/Linux/crtend.o               \
; RUN: %p/../libs/X86/Linux/crtn.o                 \
; RUN: %t.libc.so                                  \
; RUN: %p/../libs/X86/Linux/libgcc_s.so.1          \
; RUN: %p/../libs/X86/Linux/libm.so.6              \
; RUN: %p/../libs/X86/Linux/libstdc++.so.6

; create a fake libc.so to test looking for input in the sysroot prefix
; RUN: echo "/* GNU ld script                                            \
; RUN:       Use the shared library, but some functions are only in      \
; RUN:       the static library, so try that secondarily.  */            \
; RUN:       OUTPUT_FORMAT(elf32-i386)                                   \
; RUN:       GROUP ( /libc.so.6                                          \
; RUN:               /libc_nonshared.a                                   \
; RUN:               AS_NEEDED ( /ld-linux.so.2 ) )"                     \
; RUN: > %t.libc.so

; check using -sysroot in GROUP content resolving
; RUN: %MCLinker --eh-frame-hdr -z relro -o %t.out \
; RUN: -mtriple=x86-linux-gnu -march=x86           \
; RUN: --dynamic-linker=/lib/ld-linux.so.2         \
; RUN: --sysroot=%p/../libs/X86/Linux              \
; RUN: %p/../libs/X86/Linux/crt1.o                 \
; RUN: %p/../libs/X86/Linux/crti.o                 \
; RUN: %p/../libs/X86/Linux/crtbegin.o             \
; RUN: %p/../Exceptions/Itanium/exception_main.o   \
; RUN: %p/../libs/X86/Linux/crtend.o               \
; RUN: %p/../libs/X86/Linux/crtn.o                 \
; RUN: %t.libc.so                                  \
; RUN: %p/../libs/X86/Linux/libgcc_s.so.1          \
; RUN: %p/../libs/X86/Linux/libm.so.6              \
; RUN: %p/../libs/X86/Linux/libstdc++.so.6
