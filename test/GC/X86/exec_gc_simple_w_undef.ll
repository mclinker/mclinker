; RUN: %MCLinker -march=x86                                              \
; RUN: -dynamic-linker /lib/ld-linux.so.2                                \
; RUN: %p/../../libs/X86/Linux/crt1.o                                    \
; RUN: %p/../../libs/X86/Linux/crti.o %p/../../libs/X86/Linux/crtbegin.o \
; RUN: %p/obj/simple.o     %p/../../libs/X86/Linux/crtend.o              \
; RUN: %p/../../libs/X86/Linux/crtn.o %p/../../libs/X86/Linux/libc.so.6  \
; RUN: %p/../../libs/X86/Linux/libc_nonshared.a                          \
; RUN: --gc-sections -o %t.out -u foo -u mclinker

; foo and bar should be garbage collected
; RUN: readelf -s %t.out | FileCheck %s
; CHECK-NOT: bar
; CHECK: foo
; CHECK: main
; CHECK: mclinker
; CHECK-NOT: bar
; CHECK-NOT: foo

