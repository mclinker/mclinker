# RUN: yaml2obj -format=elf -docnum 1 %s > %t.lib.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t.bar.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t.main.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.lib.so %t.lib.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -o %t.out \
# RUN:           %t.bar.o %t.main.o %t.lib.so
# RUN: readelf -a %t.out | FileCheck %s

# CHECK: ELF Header:
# CHECK: ABI Version: 1
# CHECK: Flags: 0x{{[0-9a-f]+}}, noreorder, cpic, o32, mips32r2

# CHECK: Section Headers:
# CHECK: .rel.plt REL      {{0*}}[[RELPLT:[0-9a-f]+]]
# CHECK: .plt     PROGBITS {{0*}}[[PLT:[0-9a-f]+]]
# CHECK: .got     PROGBITS {{0*}}[[GOT:[0-9a-f]+]]
# CHECK: .got.plt PROGBITS {{0*}}[[GOTPLT:[0-9a-f]+]]
# CHECK: .bss     NOBITS   {{0*}}[[BSS:[0-9a-f]+]]

# CHECK: Dynamic section at offset
# CHECK: 0x{{[0-9a-f]+}} (MIPS_PLTGOT) 0x[[GOTPLT]]
# CHECK: 0x{{[0-9a-f]+}} (PLTREL)      REL
# CHECK: 0x{{[0-9a-f]+}} (JMPREL)      0x[[RELPLT]]
# CHECK: 0x{{[0-9a-f]+}} (PLTRELSZ)    8 (bytes)

# CHECK: Relocation section '.rel.dyn' at offset {{0x[0-9a-f]+}} contains 1 entries:
# CHECK: {{0*}}[[BSS]] {{[0-9a-f]+}} R_MIPS_COPY {{0*}}[[BSS]] environ

# CHECK: Relocation section '.rel.plt' at offset {{0x[0-9a-f]+}} contains 1 entries:
# CHECK: {{[0-9a-f]+}} {{[0-9a-f]+}} R_MIPS_JUMP_SLOT  00000000   puts

# CHECK: Symbol table '.symtab' contains 17 entries:
# CHECK: {{[0-9]+}}: {{0*}}[[GOT]]  0 OBJECT LOCAL HIDDEN  {{[0-9]+}} _GLOBAL_OFFSET_TABLE_
# CHECK: {{[0-9]+}}: {{0*}}[[PLT]]  0 OBJECT LOCAL HIDDEN  {{[0-9]+}} _PROCEDURE_LINKAGE_TABLE_
# CHECK: {{[0-9]+}}: {{[0-9a-f]+}} 16 FUNC   LOCAL DEFAULT {{[0-9]+}} __bar_pic@island-0

# CEHCK: PLT GOT:
# CHECK:  Reserved entries:
# CEHCK:    Address  Initial Purpose
# CHECK:   {{0*}}[[GOTPLT]] 00000000 PLT lazy resolver
# CHECK:   {{[0-9a-f]+}}    00000000 Module pointer
# CHECK:  Entries:
# CHECK:   Address  Initial Sym.Val. Type    Ndx Name
# CHECK:  {{[0-9a-f]+}} {{0*}}[[PLT]] 00000000 FUNC    UND puts

# lib.o
---
FileHeader:      
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC,
             EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:        
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          4

  - Name:          .data
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC ]
    AddressAlign:  16
    Size:          4

Symbols:         
  Global:          
    - Name:     puts
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     4
    - Name:     environ
      Type:     STT_OBJECT
      Section:  .data
      Value:    0
      Size:     4

# bar.o
---
FileHeader:      
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_CPIC, EF_MIPS_ABI_O32,
             EF_MIPS_ARCH_32R2 ]
Sections:        
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [SHF_ALLOC, SHF_EXECINSTR]
    AddressAlign:  16
    Size:          144

  - Name:          .rel.text
    Type:          SHT_REL
    Link:          .symtab
    AddressAlign:  4
    Info:          .text
    Relocations:     
      - Offset:  0x34
        Symbol:  .text
        Type:    R_MIPS_26
      - Offset:  0x68
        Symbol:  .text
        Type:    R_MIPS_26

  - Name:          .pdr
    Type:          SHT_PROGBITS
    AddressAlign:  4
    Size:          96

  - Name:          .rel.pdr
    Type:          SHT_REL
    Link:          .symtab
    AddressAlign:  4
    Info:          .pdr
    Relocations:     
      - Offset:  0x00
        Symbol:  .text
        Type:    R_MIPS_32
      - Offset:  0x20
        Symbol:  .text
        Type:    R_MIPS_32
      - Offset:  0x40
        Symbol:  bar
        Type:    R_MIPS_32

Symbols:         
  Local:           
    - Name:            .text
      Type:            STT_SECTION
      Section:         .text
    - Name:            bar1
      Type:            STT_FUNC
      Section:         .text
      Size:            0x24
    - Name:            bar2
      Type:            STT_FUNC
      Section:         .text
      Value:           0x24
      Size:            0x34
  Global:          
    - Name:            bar
      Type:            STT_FUNC
      Section:         .text
      Value:           0x58
      Size:            0x30

# main.o
---
FileHeader:      
  Class:     ELFCLASS32
  Data:      ELFDATA2LSB
  Type:      ET_REL
  Machine:   EM_MIPS
  Flags:     [EF_MIPS_NOREORDER, EF_MIPS_CPIC, EF_MIPS_ABI_O32,
              EF_MIPS_ARCH_32R2]

Sections:        
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          192

  - Name:          .rel.text
    Type:          SHT_REL
    Link:          .symtab
    AddressAlign:  4
    Info:          .text
    Relocations:     
      - Offset:          0x18
        Symbol:          puts
        Type:            R_MIPS_26
      - Offset:          0x48
        Symbol:          .text
        Type:            R_MIPS_26
      - Offset:          0x50
        Symbol:          environ
        Type:            R_MIPS_HI16
      - Offset:          0x54
        Symbol:          environ
        Type:            R_MIPS_LO16
      - Offset:          0x60
        Symbol:          puts
        Type:            R_MIPS_26
      - Offset:          0x88
        Symbol:          puts
        Type:            R_MIPS_26
      - Offset:          0x90
        Symbol:          bar
        Type:            R_MIPS_26

  - Name:            .pdr
    Type:            SHT_PROGBITS
    AddressAlign:    4
    Size:            64

  - Name:            .rel.pdr
    Type:            SHT_REL
    Link:            .symtab
    AddressAlign:    4
    Info:            .pdr
    Relocations:     
      - Offset:          0x00
        Symbol:          .text
        Type:            R_MIPS_32
      - Offset:          0x20
        Symbol:          main
        Type:            R_MIPS_32

Symbols:         
  Local:           
    - Name:            .text
      Type:            STT_SECTION
      Section:         .text
    - Name:            foo
      Type:            STT_FUNC
      Section:         .text
      Size:            0x38
  Global:          
    - Name:            puts
    - Name:            main
      Type:            STT_FUNC
      Section:         .text
      Value:           0x38
      Size:            0x7C
    - Name:            environ
    - Name:            bar
...
