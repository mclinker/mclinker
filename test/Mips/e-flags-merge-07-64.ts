# Check that the linker does not allow to mix nan2008
# and legacy MIPS object files in case of N64 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-2008.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-legacy.o

# RUN: not %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so \
# RUN:               %t-2008.o %t-legacy.o 2>&1 | FileCheck %s

# CHECK: target -mnan=2008 flag is incompatible with -mnan=legacy in e-flags-merge-07-64

# 2008.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_64, EF_MIPS_NAN2008]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8

# legacy.o
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
