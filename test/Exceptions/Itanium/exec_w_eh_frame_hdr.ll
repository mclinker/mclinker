; RUN: %MCLinker --eh-frame-hdr -z relro -mtriple=x86-linux-gnu          \
; RUN: -dynamic-linker /lib/ld-linux.so.2                                \
; RUN: -march=x86 %p/../../libs/X86/Linux/crt1.o                         \
; RUN: %p/../../libs/X86/Linux/crti.o %p/../../libs/X86/Linux/crtbegin.o \
; RUN: %p/exception_main.o %p/../../libs/X86/Linux/crtend.o              \
; RUN: %p/../../libs/X86/Linux/crtn.o %p/../../libs/X86/Linux/libc.so.6  \
; RUN: %p/../../libs/X86/Linux/libc_nonshared.a                          \
; RUN: --as-needed                                                       \
; RUN: %p/../../libs/X86/Linux/ld-linux.so.2                             \
; RUN: --no-as-needed                                                    \
; RUN: %p/../../libs/X86/Linux/libgcc_s.so.1                             \
; RUN: %p/../../libs/X86/Linux/libm.so.6                                 \
; RUN: %p/../../libs/X86/Linux/libstdc++.so.6 -o %t.out


; check GNU_EH_FRAME segment address
; get .eh_frame_hdr address
; RUN: readelf -S %t.out | grep -o "\.eh_frame_hdr *PROGBITS *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t.txt

; get segment address
; RUN: readelf -l %t.out | grep "GNU_EH_FRAME [0-9a-f]*" | \
; RUN: awk '{print $3}' >> %t.txt

; compare these two address, they should be the same
; RUN: cat %t.txt | FileCheck %s -check-prefix=SEG
; SEG: [[ADDR:([0-9a-f]*)]]
; SEG-NEXT: 0x[[ADDR]]


; check .eh_frame_hdr size
; RUN: readelf -S %t.out | \
; RUN: grep -o "\.eh_frame_hdr *PROGBITS *[0-9a-f]* *[0-9a-f]* *[0-9a-f]*" | \
; RUN: awk '{print $5}' | FileCheck %s -check-prefix=SIZE
; SIZE: 000034

