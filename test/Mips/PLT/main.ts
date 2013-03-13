; RUN: %MCLinker -march=mipsel -filetype=exe \
; RUN: %p/../../libs/MIPS/Linux/crt1.o %p/../../libs/MIPS/Linux/crti.o \
; RUN: %p/main.o %p/bar.o %p/../../libs/MIPS/Linux/libc_nonshared.a \
; RUN: %p/../../libs/MIPS/Linux/ld.so.1 \
; RUN: %p/../../libs/MIPS/Linux/crtn.o \
; RUN: %p/../../libs/MIPS/Linux/libc.so.6 -o %t.out

; RUN: readelf -a %t.out | FileCheck %s
;
; CHECK: ELF Header:
; CHECK: ABI Version: 1
; CHECK: Flags: 0x{{[0-9a-f]+}}, noreorder, cpic, o32, mips32r2

; CHECK: Section Headers:
; CHECK: .rel.plt REL      {{0*}}[[RELPLT:[0-9a-f]+]]
; CHECK: .plt     PROGBITS {{0*}}[[PLT:[0-9a-f]+]]
; CHECK: .got     PROGBITS {{0*}}[[GOT:[0-9a-f]+]]
; CHECK: .got.plt PROGBITS {{0*}}[[GOTPLT:[0-9a-f]+]]
; CHECK: .bss     NOBITS   {{0*}}[[BSS:[0-9a-f]+]]

; CHECK: Dynamic section at offset
; CHECK: 0x{{[0-9a-f]+}} (MIPS_PLTGOT) 0x[[GOTPLT]]
; CHECK: 0x{{[0-9a-f]+}} (PLTREL)      REL
; CHECK: 0x{{[0-9a-f]+}} (JMPREL)      0x[[RELPLT]]
; CHECK: 0x{{[0-9a-f]+}} (PLTRELSZ)    16 (bytes)

; CHECK: Relocation section '.rel.dyn' at offset {{0x[0-9a-f]+}} contains 1 entries:
; CHECK: {{0*}}[[BSS]] {{[0-9a-f]+}} R_MIPS_COPY {{0*}}[[BSS]] environ

; CHECK: Relocation section '.rel.plt' at offset {{0x[0-9a-f]+}} contains 2 entries:
; CHECK: {{[0-9a-f]+}} {{[0-9a-f]+}} R_MIPS_JUMP_SLOT  00000000   __libc_start_main
; CHECK: {{[0-9a-f]+}} {{[0-9a-f]+}} R_MIPS_JUMP_SLOT  00000000   puts

; CHECK: Symbol table '.symtab' contains 54 entries:
; CHECK: {{[0-9]+}}: {{0*}}[[GOT]]  0 OBJECT LOCAL HIDDEN  {{[0-9]+}} _GLOBAL_OFFSET_TABLE_
; CHECK: {{[0-9]+}}: {{0*}}[[PLT]]  0 OBJECT LOCAL HIDDEN  {{[0-9]+}} _PROCEDURE_LINKAGE_TABLE_
; CHECK: {{[0-9]+}}: {{[0-9a-f]+}} 16 FUNC   LOCAL DEFAULT {{[0-9]+}} __stat_pic@island-0
; CHECK: {{[0-9]+}}: {{[0-9a-f]+}} 16 FUNC   LOCAL DEFAULT {{[0-9]+}} __bar_pic@island-0

; CEHCK: PLT GOT:
; CHECK:  Reserved entries:
; CEHCK:    Address  Initial Purpose
; CHECK:   {{0*}}[[GOTPLT]] 00000000 PLT lazy resolver
; CHECK:   {{[0-9a-f]+}}    00000000 Module pointer
; CHECK:  Entries:
; CHECK:   Address  Initial Sym.Val. Type    Ndx Name
; CHECK:  {{[0-9a-f]+}} {{0*}}[[PLT]] 00000000 FUNC    UND __libc_start_main
; CHECK:  {{[0-9a-f]+}} {{0*}}[[PLT]] 00000000 FUNC    UND puts
