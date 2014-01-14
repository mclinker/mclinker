; RUN: %MCLinker -z relro --eh-frame-hdr -mtriple=x86-linux-gnu     \
; RUN: -dynamic-linker /lib/ld-linux.so.2                           \
; RUN: -march=x86 %p/../../../libs/X86/Linux/crt1.o                 \
; RUN: %p/../../../libs/X86/Linux/crti.o                            \
; RUN: %p/../../../libs/X86/Linux/crtbegin.o                        \
; RUN: %p/obj/tls_main.o %p/obj/tls_foo_pic.o                       \
; RUN: %p/../../../libs/X86/Linux/crtend.o                          \
; RUN: %p/../../../libs/X86/Linux/crtn.o                            \
; RUN: %p/../../../libs/X86/Linux/libc_nonshared.a                  \
; RUN: --as-needed                                                  \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                     \
; RUN: --no-as-needed                                               \
; RUN: %p/../../../libs/X86/Linux/libc.so.6                         \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                     \
; RUN: -o %t.exe

; check relocation type
; RUN: readelf -r %t.exe | FileCheck %s -check-prefix=REL
; REL: R_386_TLS_DTPMOD3 {{[0-9a-fA-F]+}} .tbss
; REL-NEXT: R_386_TLS_DTPMOD3 {{[0-9a-fA-F]+}} .tdata
; REL-NEXT: R_386_TLS_DTPMOD3 {{[0-9a-fA-F]+}} tls2
; REL-NEXT: R_386_TLS_DTPOFF3 {{[0-9a-fA-F]+}} tls2
; REL-NEXT: R_386_TLS_DTPMOD3 {{[0-9a-fA-F]+}} tls_nodef
; REL-NEXT: R_386_TLS_DTPOFF3 {{[0-9a-fA-F]+}} tls_nodef
; REL-NEXT: R_386_TLS_DTPMOD3 {{[0-9a-fA-F]+}} tls1
; REL-NEXT: R_386_TLS_DTPOFF3 {{[0-9a-fA-F]+}} tls1
