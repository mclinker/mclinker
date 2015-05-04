# Check handling of R_MIPS_64 relocation.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mips64el-unknown-linux -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s -t %t.exe | FileCheck %s

# CHECK: Contents of section .data:
# CHECK-NEXT: 120010000 f4000020 01000000 f0000020 01000100
#                       ^ T0 + 4 = 0x1200000f4
#                                         ^ T0 + 0x1000000000000
#                                              = 0x10001200000f0
# CHECK: SYMBOL TABLE:
# CHECK: 00000001200000f0  .text  00000008 T0

FileHeader:
  Class:   ELFCLASS64
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ARCH_64]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Size:         0x08
  AddressAlign: 16
  Flags:        [SHF_ALLOC]
- Name:         .data
  Type:         SHT_PROGBITS
  Size:         0x10
  AddressAlign: 16
  Flags:        [SHF_ALLOC, SHF_WRITE]

- Name:         .rela.data
  Type:         SHT_RELA
  Info:         .data
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T0
      Type:   R_MIPS_64
      Addend: 4
    - Offset: 8
      Symbol: T0
      Type:   R_MIPS_64
      Addend: 0x1000000000000

Symbols:
  Global:
    - Name:    T0
      Section: .text
      Value:   0x0
      Size:    8
    - Name:    data1
      Section: .data
      Value:   0x0
      Size:    8
    - Name:    data2
      Section: .data
      Value:   0x8
      Size:    8
