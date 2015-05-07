# Check that mixing 32 and 64-bit MIPS object files
# is not allowed in case of N64 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-32.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-64.o

# RUN: not %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so \
# RUN:               %t-32.o %t-64.o 2>&1 | FileCheck %s

# CHECK: target 'ELFCLASS64' is incompatible with 'ELFCLASS32' in e-flags-merge-05-64

# 32.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

# 64.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_64]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8
...
