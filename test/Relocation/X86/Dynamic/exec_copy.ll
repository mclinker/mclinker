; RUN: %MCLinker -march=x86 -mtriple=x86-linux-gnu           \
; RUN: -dynamic-linker /lib/ld-linux.so.2                    \
; RUN: %p/../../../libs/X86/Linux/crt1.o                     \
; RUN: %p/../../../libs/X86/Linux/crti.o                     \
; RUN: %p/main.o                                             \
; RUN: %p/../../../libs/X86/Linux/libc_nonshared.a           \
; RUN: --as-needed                                           \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2              \
; RUN: %p/../../../libs/X86/Linux/crtn.o                     \
; RUN: %p/../../../libs/X86/Linux/libc.so.6 -o %t.exe

; RUN: readelf -r %t.exe | FileCheck %s -check-prefix=REL
; REL: R_386_COPY {{[0-9a-fA-F]+}} {{tzname|__tzname}}

; This fails due to unsupported weak alias in shared object:
; http://code.google.com/p/mclinker/issues/detail?id=123
; readelf -s -D %t.exe | FileCheck %s -check-prefix=SYM
; SYM: 8 OBJECT  GLOBAL DEFAULT  {{[0-9]+}} __tzname
