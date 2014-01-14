; RUN: %MCLinker -march=x86                                              \
; RUN: -shared %p/obj/foo.o -o %t.foo.so

; RUN: %MCLinker -march=x86                                              \
; RUN: -dynamic-linker /lib/ld-linux.so.2                                \
; RUN: %p/../../libs/X86/Linux/crt1.o                                    \
; RUN: %p/../../libs/X86/Linux/crti.o %p/../../libs/X86/Linux/crtbegin.o \
; RUN: %p/obj/main.o %t.foo.so %p/../../libs/X86/Linux/crtend.o          \
; RUN: %p/../../libs/X86/Linux/crtn.o %p/../../libs/X86/Linux/libc.so.6  \
; RUN: --gc-sections -o %t.out

; foo and bar should be garbage collected
; RUN: readelf -s %t.out | FileCheck %s
; CHECK: main
; CHECK: bar
; CHECK: foo

