; RUN: %MCLinker -z relro --eh-frame-hdr -mtriple=x86-linux-gnu           \
; RUN: -dynamic-linker /lib/ld-linux.so.2                                 \
; RUN: -march=x86 %p/../../../libs/X86/Linux/crt1.o                       \
; RUN: %p/../../../libs/X86/Linux/crti.o                                  \
; RUN: %p/../../../libs/X86/Linux/crtbegin.o                              \
; RUN: %p/obj/tls_main.o                                                  \
; RUN: %p/obj/tls_foo_extern_gotie.o                                      \
; RUN: %p/obj/tls_variables.o                                             \
; RUN: %p/../../../libs/X86/Linux/crtend.o                                \
; RUN: %p/../../../libs/X86/Linux/crtn.o                                  \
; RUN: %p/../../../libs/X86/Linux/libc_nonshared.a                        \
; RUN: --as-needed                                                        \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                           \
; RUN: --no-as-needed                                                     \
; RUN: %p/../../../libs/X86/Linux/libc.so.6                               \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                           \
; RUN: -o %t.exe


; check relocation type
; RUN: readelf -r %t.exe | FileCheck %s -check-prefix=REL
; REL: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} tls_nodef
; REL-NEXT: R_386_TLS_TPOFF {{[0-9a-fA-F]+}} tls_def


; check the TLS segment
; get .tdata address
; RUN: readelf -S %t.exe | grep -o "\.tdata *PROGBITS *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t.txt

; get the TLS segement address
; RUN: readelf -l %t.exe | grep "TLS [0-9a-f]*" | \
; RUN: awk '{print $3}' >> %t.txt

; RUN: cat %t.txt | FileCheck %s -check-prefix=SEG
; SEG: [[ADDR:([0-9a-f]*)]]
; SEG-NEXT: 0x[[ADDR]]

