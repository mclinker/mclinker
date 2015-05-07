# Check that the linker does not allow to mix nan2008
# and legacy MIPS object files in case of O32 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-2008.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-legacy.o

# RUN: not %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so \
# RUN:               %t-2008.o %t-legacy.o 2>&1 | FileCheck %s

# CHECK: target -mnan=2008 flag is incompatible with -mnan=legacy in e-flags-merge-07

# 2008.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32, EF_MIPS_NAN2008]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

# legacy.o
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
...
