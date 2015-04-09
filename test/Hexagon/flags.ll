; RUN: %MCLinker -march=hexagon -mcpu=hexagonv4 -mtriple=hexagon-none-linux \
; RUN: %p/../libs/Hexagon/v4objs/flags.o  \
; RUN: -o %t1.out

; check if the relocation has been fixed up
; RUN: readelf -e  %t1.out | grep 'Flags:' | head -1

; CHECK: Flags:                             0x3

; RUN: %MCLinker -march=hexagon -mcpu=hexagonv5 -mtriple=hexagon-none-linux \
; RUN: %p/../libs/Hexagon/v4objs/flags.o  \
; RUN: -o %t2.out

; check if the relocation has been fixed up
; RUN: readelf -e  %t2.out | grep 'Flags:' | head -1

; CHECK: Flags:                             0x4
