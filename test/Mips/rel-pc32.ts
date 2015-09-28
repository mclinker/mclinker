# Check handling of R_MIPS_PC32 relocation.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-unknown-linux -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s -t %t.exe | FileCheck %s

# CHECK: Contents of section .data:
# CHECK-NEXT: {{[0-9A-F]+}} 00000000 05000080 fdffff7f
#                                    ^^ data2 + 0x80000001 - data1
#                                             ^^ data1 + 0x80000001 - data2
# CHECK: SYMBOL TABLE:
# CHECK: {{[0-9A-F]+}} .data  00000004 data1
# CHECK: {{[0-9A-F]+}} .data  00000004 data2

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  AddressAlign: 16
  Flags:        [SHF_ALLOC]
  Size:         4

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
      Symbol: data2
      Type:   R_MIPS_PC32
    - Offset: 8
      Symbol: data1
      Type:   R_MIPS_PC32

Symbols:
  Global:
    - Name:    __start
      Section: .text
      Value:   0
      Size:    4
    - Name:    data1
      Section: .data
      Value:   4
      Size:    4
    - Name:    data2
      Section: .data
      Value:   8
      Size:    4
