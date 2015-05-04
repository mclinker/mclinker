# Check R_MIPS_GPREL32/R_MIPS_64 and R_MIPS_GPREL16/R_MIPS_64
# relocations handling.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mips64el-unknown-linux -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s %t.exe | FileCheck %s
# RUN: llvm-nm %t.exe | FileCheck --check-prefix=SYM %s

# CHECK:      Contents of section .text:
# CHECK-NEXT:  1200000f0 1881feff ffffffff 1881feff ffffffff
# CHECK-NEXT:  120000100 1881feff ffffffff 1881feff ffffffff

# SYM: 0000000120000108 T LT1
# SYM: 0000000120010000 d _GLOBAL_OFFSET_TABLE_

---
FileHeader:
  Class:   ELFCLASS64
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ARCH_64]

Sections:
  - Name:            .text
    Type:            SHT_PROGBITS
    Flags:           [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:    16
    Size:            32

  - Name:            .rela.text
    Type:            SHT_RELA
    Link:            .symtab
    AddressAlign:    8
    Info:            .text
    Relocations:
      - Offset: 0
        Symbol: LT1
        Type:   R_MIPS_GPREL16
        Type2:  R_MIPS_64
        Type3:  R_MIPS_NONE
      - Offset: 8
        Symbol: LT1
        Type:   R_MIPS_GPREL16
        Type2:  R_MIPS_64
        Type3:  R_MIPS_NONE
      - Offset: 16
        Symbol: LT1
        Type:   R_MIPS_GPREL32
        Type2:  R_MIPS_64
        Type3:  R_MIPS_NONE
      - Offset: 24
        Symbol: LT1
        Type:   R_MIPS_GPREL32
        Type2:  R_MIPS_64
        Type3:  R_MIPS_NONE

Symbols:
  Local:
    - Name:    .text
      Type:    STT_SECTION
      Section: .text

  Global:
    - Name:    LT1
      Type:    STT_FUNC
      Section: .text
      Value:   24
      Size:    8
    - Name:    T0
      Type:    STT_FUNC
      Section: .text
      Value:   0
      Size:    24
...
