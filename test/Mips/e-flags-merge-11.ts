# Check that Mclinker shows an error and does not link files with mips64r2
# and mips64r6 instructions sets.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-64r2.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-64r6.o

# RUN: not %MCLinker -mtriple=mips64el-unknown-linux -o %t.exe \
# RUN:         %t-64r2.o %t-64r6.o 2>&1 | FileCheck %s

# CHECK: target arch 'mips64r2' is inconsist with the 'mips64r6' in e-flags-merge-11

# 64r2.o
---
FileHeader:
  Class:           ELFCLASS64
  Data:            ELFDATA2LSB
  Type:            ET_REL
  Machine:         EM_MIPS
  Flags:           [EF_MIPS_ARCH_64R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  0x04
    Size:          0x04

# 64r6.o
---
FileHeader:
  Class:           ELFCLASS64
  Data:            ELFDATA2LSB
  Type:            ET_REL
  Machine:         EM_MIPS
  Flags:           [EF_MIPS_ARCH_64R6]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  0x04
    Size:          0x04
...
