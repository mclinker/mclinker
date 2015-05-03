# Check handling multiple relocations packed
# in the single MIPS N64 ABI relocation record.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-so.o
# RUN: %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so %t-so.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-o.o
# RUN: %MCLinker -mtriple=mips64el-linux-gnu -e T0 -o %t.exe %t-o.o %t.so
# RUN: llvm-readobj -s -t -mips-plt-got %t.exe | FileCheck -check-prefix=SYM %s
# RUN: llvm-objdump -s %t.exe | FileCheck -check-prefix=RAW %s

# SYM:      Section {
# SYM:        Index: 6
# SYM-NEXT:   Name: .rodata
# SYM-NEXT:   Type: SHT_PROGBITS
# SYM-NEXT:   Flags [
# SYM-NEXT:     SHF_ALLOC
# SYM-NEXT:   ]
# SYM-NEXT:   Address: 0x1200002E0
#
# SYM:      Symbol {
# SYM:        Name: LT1
# SYM-NEXT:   Value: 0x1200002C0
# SYM-NEXT:   Size: 16
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: .text
# SYM-NEXT: }
# SYM:      Symbol {
# SYM:        Name: T0
# SYM-NEXT:   Value: 0x1200002D0
# SYM-NEXT:   Size: 16
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: .text
# SYM-NEXT: }
# SYM:      Symbol {
# SYM:        Name: T1
# SYM-NEXT:   Value: 0x0
# SYM-NEXT:   Size: 0
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: Undefined
# SYM-NEXT: }
#
# SYM:      Canonical gp value: 0x120018110
# SYM:      Local entries [
# SYM-NEXT:   Entry {
# SYM-NEXT:     Address: 0x120010130
# SYM-NEXT:     Access: -32736
# SYM-NEXT:     Initial: 0x1200002E8
# SYM-NEXT:   }
# SYM-NEXT: ]
# SYM-NEXT: Global entries [
# SYM-NEXT:   Entry {
# SYM-NEXT:     Address: 0x120010138
# SYM-NEXT:     Access: -32728
# SYM-NEXT:     Initial: 0x0
# SYM-NEXT:     Value: 0x0
# SYM-NEXT:     Type: Function (0x2)
# SYM-NEXT:     Section: Undefined (0x0)
# SYM-NEXT:     Name: T1@ (32)
# SYM-NEXT:   }
# SYM-NEXT: ]

# RAW:      Contents of section .text:
# RAW-NEXT:  1200002c0 01000000 00000000 507e0000 00000000
#                      ^
#                      S - GP = 0x1200002C0 - 0x120018110 = -97872
#                      S - A = 0 - (-97872) = 97872
#                      (AHL + S + 0x8000) >> 16 = 1
#                                        ^
#                      S - GP = 0x1200002C0 - 0x120018110 = -97872
#                      S - A = 0 - (-97872) = 97872
#                      AHL + S = 0x7e50
# RAW-NEXT:  1200002d0 20800000 00000000 28800000 00000000
#                      ^ 0x8020 = -32736 GOT
#                                        ^ 0x8028 = -32728
# RAW:      Contents of section .pdr:
# RAW-NEXT:  0000 c0020020 d0020020

---
FileHeader:
  Class:   ELFCLASS64
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ARCH_64]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8

Symbols:
  Local:
    - Name:     .text
      Type:     STT_SECTION
      Section:  .text

  Global:
    - Name:     T1
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     8

---
FileHeader:
  Class:   ELFCLASS64
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ARCH_64]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          32

  - Name:          .rela.text
    Type:          SHT_RELA
    Link:          .symtab
    AddressAlign:  8
    Info:          .text
    Relocations:
      - Offset:  0
        Symbol:  LT1
        Type:    R_MIPS_GPREL16
        Type2:   R_MIPS_SUB
        Type3:   R_MIPS_HI16
      - Offset:  8
        Symbol:  LT1
        Type:    R_MIPS_GPREL16
        Type2:   R_MIPS_SUB
        Type3:   R_MIPS_LO16
      - Offset:  16
        Symbol:  .rodata
        Type:    R_MIPS_GOT_PAGE
        Addend:  8
      - Offset:  20
        Symbol:  .rodata
        Type:    R_MIPS_GOT_OFST
        Addend:  8
      - Offset:  24
        Symbol:  T1
        Type:    R_MIPS_CALL16

  - Name:          .pdr
    Type:          SHT_PROGBITS
    AddressAlign:  4
    Size:          8

  - Name:          .rela.pdr
    Type:          SHT_RELA
    Link:          .symtab
    AddressAlign:  8
    Info:          .pdr
    Relocations:
      - Offset:  0
        Symbol:  LT1
        Type:    R_MIPS_32
      - Offset:  4
        Symbol:  T0
        Type:    R_MIPS_32

  - Name:          .rodata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC ]
    AddressAlign:  16
    Size:          16

Symbols:
  Local:
    - Name:     .text
      Type:     STT_SECTION
      Section:  .text
    - Name:     .rodata
      Type:     STT_SECTION
      Section:  .rodata
    - Name:     .pdr
      Type:     STT_SECTION
      Section:  .pdr

  Global:
    - Name:     LT1
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     16
    - Name:     T0
      Type:     STT_FUNC
      Section:  .text
      Value:    16
      Size:     16
    - Name:     T1
...
