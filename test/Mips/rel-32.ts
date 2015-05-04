# Check handling of R_MIPS_32 relocation.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-unknown-linux -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s -t %t.exe | FileCheck %s

# CHECK: Contents of section .data:
# CHECK-NEXT: 410000 00000000 09004180 05004180
#                             ^^ D2 + 0x80000001 = 0x80410009
#                                      ^^ D1 + 0x80000001 = 0x80410005
# CHECK: SYMBOL TABLE:
# CHECK: 00410004  .data  00000004 D1
# CHECK: 00410008  .data  00000004 D2

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Size:         4
  AddressAlign: 16
  Flags:        [SHF_ALLOC]

- Name:         .data
  Type:         SHT_PROGBITS
  Content:      "000000000100008001000080"
  AddressAlign: 16
  Flags:        [SHF_ALLOC, SHF_WRITE]

- Name:         .rel.data
  Type:         SHT_REL
  Info:         .data
  AddressAlign: 4
  Relocations:
    - Offset: 4
      Symbol: D2
      Type:   R_MIPS_32
    - Offset: 8
      Symbol: D1
      Type:   R_MIPS_32

Symbols:
  Global:
    - Name:    T0
      Section: .text
      Value:   0
      Size:    4
    - Name:    D1
      Section: .data
      Value:   4
      Size:    4
    - Name:    D2
      Section: .data
      Value:   8
      Size:    4
