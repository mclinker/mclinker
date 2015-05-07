# Check selecting ELF header ARCH flag in case of O32 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-m1.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-m2.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-m32.o
# RUN: yaml2obj -format=elf -docnum 4 %s > %t-m32r2.o

# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so \
# RUN:     %t-m32.o %t-m2.o %t-m32r2.o %t-m1.o
# RUN: llvm-readobj -file-headers %t.so | FileCheck %s

# CHECK:      Flags [
# CHECK-NEXT:   EF_MIPS_ABI_O32
# CHECK-NEXT:   EF_MIPS_ARCH_32R2
# CHECK-NEXT: ]

# m1.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_1]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

# m2.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

# m32.o
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

# m32r2.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4
...
