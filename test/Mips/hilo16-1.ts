# Check handling multiple HI16 relocation followed by a single LO16 relocation.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s -t %t.exe | FileCheck %s

# CHECK: Contents of section .text:
# CHECK-NEXT: 4000a0 40000000 40000000 40000000 40000000
# CHECK-NEXT: 4000b0 42000000 3e000000 40020000 40fe0000
# CHECK-NEXT: 4000c0 c5000000 00000000

# CHECK: SYMBOL TABLE:
# CHECK: 004000a0 g F .text  00000024 T0
# CHECK: 004000c4 g F .text  00000004 T1

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Content:      "0000000000000000000000000000000002000000feff00000002000000fe00000100000000000000"
#                ^ %hi(T1+1)     ^ %hi(T1+0x100) ^ %hi(T1+0x20000)       ^ %hi(T1-0x1ffffff)
#                        ^ %hi(T1-1)     ^ %hi(T1-0x100) ^ %hi(T1-0x1ffff)       ^ %lo(T1-0x1ffffff)
#                                                                ^ %hi(T1+0x2000000)
  AddressAlign: 16
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]

- Name:         .rel.text
  Type:         SHT_REL
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 4
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 8
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 12
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 16
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 20
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 24
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 28
      Symbol: T1
      Type:   R_MIPS_HI16
    - Offset: 32
      Symbol: T1
      Type:   R_MIPS_LO16

Symbols:
  Global:
    - Name:    T0
      Section: .text
      Type:    STT_FUNC
      Value:   0
      Size:    36
    - Name:    T1
      Section: .text
      Type:    STT_FUNC
      Value:   36
      Size:    4
