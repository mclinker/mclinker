; generate the shared lib
; RUN: %MCLinker -z relro --eh-frame-hdr -mtriple=x86-linux-gnu     \
; RUN: -dynamic-linker %p/../../../libs/X86/Linux/ld-linux.so.2     \
; RUN: -march=x86 -shared %p/obj/tls_foo.o -o %t.so


; generate .exe calling the shared lib
; RUN: %MCLinker -z relro --eh-frame-hdr -mtriple=x86-linux-gnu     \
; RUN: -dynamic-linker %p/../../../libs/X86/Linux/ld-linux.so.2     \
; RUN: -march=x86 %p/../../../libs/X86/Linux/crt1.o                 \
; RUN: %p/../../../libs/X86/Linux/crti.o                            \
; RUN: %p/../../../libs/X86/Linux/crtbegin.o                        \
; RUN: %p/obj/tls_main.o %t.so                                      \
; RUN: %p/../../../libs/X86/Linux/crtend.o                          \
; RUN: %p/../../../libs/X86/Linux/crtn.o                            \
; RUN: %p/../../../libs/X86/Linux/libc_nonshared.a                  \
; RUN: %p/../../../libs/X86/Linux/libc.so.6                         \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                     \
; RUN: -o %t.exe


; check relocation types
; RUN: readelf -r %t.so | FileCheck %s -check-prefix=REL
; REL: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} tls_nodef
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} static_tls_nodef
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} static_tls
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} static_tls_nodef
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} tls1
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} tls2
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} tls_nodef


; check .dynamic DT_FLAGS
; RUN: readelf -d %t.so | grep FLAGS | FileCheck %s -check-prefix=DYN
; DYN: TEXTREL
; DYN: STATIC_TLS


; check the local tls symbols have been emitted into .dynsym
; RUN: readelf --dyn-syms %t.so | FileCheck %s -check-prefix=SYM
; SYM: static_tls
; SYM: static_tls_nodef


; check the TLS segment
; get .tdata address
; RUN: readelf -S %t.so | grep -o "\.tdata *PROGBITS *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t.txt

; get the TLS segement address
; RUN: readelf -l %t.so | grep "TLS [0-9a-f]*" | \
; RUN: awk '{print $3}' >> %t.txt

; RUN: cat %t.txt | FileCheck %s -check-prefix=SEG
; SEG: [[ADDR:([0-9a-f]*)]]
; SEG-NEXT: 0x[[ADDR]]

