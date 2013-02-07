; RUN: %MCLinker -z relro --eh-frame-hdr -mtriple=x86-linux-gnu     \
; RUN: -dynamic-linker /lib/ld-linux.so.2                           \
; RUN: -march=x86 %p/../../../libs/X86/Linux/crt1.o                 \
; RUN: %p/../../../libs/X86/Linux/crti.o                            \
; RUN: %p/../../../libs/X86/Linux/crtbegin.o                        \
; RUN: %p/obj/tls_main.o %p/obj/tls_foo.o                           \
; RUN: %p/../../../libs/X86/Linux/crtend.o                          \
; RUN: %p/../../../libs/X86/Linux/crtn.o                            \
; RUN: %p/../../../libs/X86/Linux/libc_nonshared.a                  \
; RUN: --as-needed                                                  \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                     \
; RUN: --no-as-needed                                               \
; RUN: %p/../../../libs/X86/Linux/libc.so.6                         \
; RUN: %p/../../../libs/X86/Linux/ld-linux.so.2                     \
; RUN: -o %t.exe


; all TLS relocation should be static applied
; RUN: readelf -r %t.exe | FileCheck %s -check-prefix=REL
; REL-NOT: {{R_386_TLS_*}}
; REL-NOT: tls_nodef
; REL-NOT: .tbss
; REL-NOT: .tdata
; REL-NOT: tls1
; REL-NOT: tls2
; REL-NOT: static_tls_nodef
; REL-NOT: static_tls


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

