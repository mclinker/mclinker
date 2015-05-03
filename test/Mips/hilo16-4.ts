# Check pairing of R_MIPS_HI16 and R_MIPS_LO16 relocations.
#
# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T1 -o %t.exe %t.o
# RUN: llvm-objdump -t -s %t.exe | FileCheck %s

# CHECK: Contents of section .text:
# CHECK-NEXT: 4000a0 41000000 ff7f0000 00000000 04000000
# CHECK-NEXT: 4000b0 40800000

# CHECK: SYMBOL TABLE:
# CHECK: 004000b0         .text  00000004 T3
# CHECK: 00410000         .data  0000000c X
# CHECK: 004000a0         .text  00000008 T1
# CHECK: 004000a8         .text  00000008 T2

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name: .text
  Type: SHT_PROGBITS
# T1:
# lui     $4,%hi(X)             # rel A
# lw      $6,%lo(X+32767)($5)   # rel B
# T2:
# lb      $7,%lo(X)($6)         # rel C
# lb      $8,%lo(X+4)($6)       # rel D
# T3:
# lui     $5,%hi(X+32767)       # rel E
  Content:  "00000000FF7F00000000000004000000FF7F0000"
  AddressAlign: 16
  Flags:        [SHF_EXECINSTR, SHF_ALLOC]

- Name:         .data
  Type:         SHT_PROGBITS
  Size:         12
  AddressAlign: 16
  Flags:        [SHF_WRITE, SHF_ALLOC]

- Name:         .rel.text
  Type:         SHT_REL
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 16          # rel E
      Symbol: X
      Type:   R_MIPS_HI16
    - Offset: 4           # rel B
      Symbol: X
      Type:   R_MIPS_LO16
    - Offset: 0           # rel A
      Symbol: X
      Type:   R_MIPS_HI16
    - Offset: 12          # rel D
      Symbol: X
      Type:   R_MIPS_LO16
    - Offset: 8           # rel C
      Symbol: X
      Type:   R_MIPS_LO16

Symbols:
  Global:
    - Name:    T1
      Section: .text
      Value:   0
      Size:    8
    - Name:    T2
      Section: .text
      Value:   8
      Size:    8
    - Name:    T3
      Section: .text
      Value:   16
      Size:    4
    - Name:    X
      Section: .data
      Value:   0
      Size:    12
