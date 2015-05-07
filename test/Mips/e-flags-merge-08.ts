# Check that the linker accepts files with mips32 and mips64 instructions
# if all these files satisfy O32 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-32.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-64.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-64r2.o

# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so \
# RUN:           %t-32.o %t-64.o %t-64r2.o
# RUN: llvm-readobj -file-headers %t.so | FileCheck %s

# CHECK:      Flags [
# CHECK-NEXT:   EF_MIPS_32BITMODE
# CHECK-NEXT:   EF_MIPS_ABI_O32
# CHECK-NEXT:   EF_MIPS_ARCH_64R2
# CHECK-NEXT: ]


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
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_64, EF_MIPS_32BITMODE]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

# 64r2.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_64R2, EF_MIPS_32BITMODE]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4
...
