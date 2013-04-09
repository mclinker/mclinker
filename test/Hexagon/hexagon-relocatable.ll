; Build the shared library.
; RUN: %MCLinker -r -march=hexagon %p/../libs/Hexagon/v4objs/crt.o\
; RUN:  %p/../libs/Hexagon/v4objs/minicrt.o -o %t
; RUN: readelf -r %t | FileCheck -check-prefix=RELOCS %s

RELOCS: Relocation section '.rela.text' at offset 0x5344 contains 2 entries:
RELOCS: Offset Info Type Sym.Value Sym. Name + Addend
RELOCS: {{[0-9a-f]+}} {{[0-9a-f]+}} {{[a-z:]+}} {{[0-9]}} {{[0]+}} .start + c
RELOCS: {{[0-9a-f]+}} {{[0-9a-f]+}} {{[a-z:]+}} {{[0-9]}} {{[0]+}} .start + 8
