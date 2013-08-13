; RUN: %MCLinker -march hexagon -mtriple=hexagon-none-linux \
; RUN: %p/../libs/Hexagon/v4objs/definition.o  \
; RUN: %p/../libs/Hexagon/v4objs/reference.o  \
; RUN: -o %t.out

; check if the relocation has been fixed up
; RUN: readelf -x 1 %t.out | grep '\.x\.' | FileCheck %s

; CHECK: 0x00000074 00c09da0 00c00078 1ec01e96 faffff5b .......x.......[
