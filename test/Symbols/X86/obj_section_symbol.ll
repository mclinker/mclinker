; build obj
; RUN: %MCLinker -r -march=x86 -mtriple=x86-linux-gnu \
; RUN: %p/obj/main.o %p/obj/foo.o %p/obj/bar.o        \
; RUN: -o %t.o


; check the section symbol .rodata has been emitted
; get .rodata section index
; RUN: readelf -S %t.o | grep -o "\[ [0-9]\] \.rodata *PROGBITS" | \
; RUN: awk '{print $2}' >  %t.txt

; get section symbol defined index
; RUN: readelf -s %t.o | grep -o "SECTION LOCAL  DEFAULT *[0-9]*"| \
; RUN: awk '{print $4}' >>  %t.txt

; check the section symbol defined to correct section
; RUN: cat %t.txt | FileCheck %s
; CHECK: [[INDEX:([0-9]*)]]]
; CHECK: [[INDEX]]

