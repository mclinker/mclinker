; build obj
; RUN: %MCLinker -r -march=x86 -mtriple=x86-linux-gnu \
; RUN: %p/obj/foo.o %p/obj/main.o                     \
; RUN: -o %t.o

; build exe
; RUN: %MCLinker -mtriple=x86-linux-gnu                                  \
; RUN: -dynamic-linker /lib/ld-linux.so.2                                \
; RUN: -march=x86 %p/../../libs/X86/Linux/crt1.o                         \
; RUN: %p/../../libs/X86/Linux/crti.o %p/../../libs/X86/Linux/crtbegin.o \
; RUN: %t.o %p/../../libs/X86/Linux/crtend.o                             \
; RUN: %p/../../libs/X86/Linux/crtn.o %p/../../libs/X86/Linux/libc.so.6  \
; RUN: %p/../../libs/X86/Linux/libc_nonshared.a                          \
; RUN: --as-needed                                                       \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2                             \
; RUN: --no-as-needed                                                    \
; RUN: --as-needed %p/../../libs/X86/Linux/ld-linux.so.2 --no-as-needed  \
; RUN: -o %t.exe

; check .xxx.text are not merged
; RUN: readelf -S %t.o | FileCheck %s
; CHECK: .text.foo
; CHECK: .text.bar
; CHECK: .text.main


