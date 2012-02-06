; .so
; RUN: %MCLinker -march=arm -filetype=dso -Bsymbolic -dB %p/empty.bc %p/ABS32.o -o %t.so
; RUN: readelf -r %t.so | FileCheck %s -check-prefix=REL
; REL: R_ARM_RELATIVE
; REL: R_ARM_RELATIVE
; REL: R_ARM_RELATIVE
; REL: R_ARM_RELATIVE
; RUN: llvm-objdump -t %t.so > %t.txt
; RUN: llvm-objdump -s %t.so >> %t.txt
; RUN: cat %t.txt | FileCheck %s -check-prefix=REL_VAL
; REL_VAL: [[bar:[0-9a-fA-F]+]] l     F .text  00000008 bar
; REL_VAL: [[localv:[0-9a-fA-F]+]] l       .data  00000004 localv

; RUN: llvm-objdump -section-headers %t.so | awk '{print $2}' | sort | FileCheck %s -check-prefix=ARM_SO_OBJDUMP
; ARM_SO_OBJDUMP: .ARM.attributes
; ARM_SO_OBJDUMP: .dynamic
; ARM_SO_OBJDUMP: .dynstr
; ARM_SO_OBJDUMP: .dynsym
; ARM_SO_OBJDUMP: .text
