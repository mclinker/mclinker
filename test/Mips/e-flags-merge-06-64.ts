# Check selecting ELF header ARCH flag in case of N64 ABI.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-m3.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-m5.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-m64.o
# RUN: yaml2obj -format=elf -docnum 4 %s > %t-m64r2.o

# RUN: %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so \
# RUN:     %t-m64.o %t-m5.o %t-m64r2.o %t-m3.o
# RUN: llvm-readobj -file-headers %t.so | FileCheck %s

# CHECK:      Flags [
# CHECK-NEXT:   EF_MIPS_ARCH_64R2
# CHECK-NEXT: ]

# m3.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_3]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8

# m5.o
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

# m64.o
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

# m64r2.o
---
FileHeader:
  Class:    ELFCLASS64
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_ARCH_64R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8
...
