; create a fake libc.so
; RUN: echo "/* GNU ld script                                            \
; RUN:       Use the shared library, but some functions are only in      \
; RUN:       the static library, so try that secondarily.  */            \
; RUN:       OUTPUT_FORMAT(elf32-i386)                                   \
; RUN:       GROUP ( %p/../libs/X86/Linux/libc.so.6                      \
; RUN:               %p/../libs/X86/Linux/libc_nonshared.a               \
; RUN:               AS_NEEDED ( %p/../libs/X86/Linux/ld-linux.so.2 ) )" \
; RUN: > %t.libc.so

; linking
; RUN: %MCLinker --eh-frame-hdr -z relro -o %t.out \
; RUN: -mtriple=x86-linux-gnu -march=x86           \
; RUN: -dynamic-linker /lib/ld-linux.so.2          \
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

; check .eh_frame CIE/FDE for plt
; RUN: readelf -S %t.out | grep -o "\ \.plt *PROGBITS *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t.txt
; RUN: readelf -w %t.out | grep pc= | tail -n 1 | awk -F '='  '{print $3}' | \
; RUN: awk -F '.' '{print $1}' >> %t.txt
; RUN: cat %t.txt | FileCheck %s -check-prefix=EHPLT
; EHPLT: [[ADDR:([0-9a-f]*)]]
; EHPLT-NEXT: [[ADDR]]
