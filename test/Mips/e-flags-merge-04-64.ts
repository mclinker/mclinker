# Check ELF flags merging in case of N64 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-none.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-noreorder.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-micro.o

# RUN: %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so \
# RUN:     %t-none.o %t-noreorder.o %t-micro.o
# RUN: llvm-readobj -file-headers %t.so | FileCheck %s

# CHECK:      Flags [
# CHECK-NEXT:   EF_MIPS_ARCH_64R2
# CHECK-NEXT:   EF_MIPS_MICROMIPS
# CHECK-NEXT:   EF_MIPS_NOREORDER
# CHECK-NEXT: ]

# none.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_5]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8

# noreorder.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_64, EF_MIPS_NOREORDER]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8

# micro.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_64R2, EF_MIPS_MICROMIPS]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8
...
