# Check handling of HI16 and LO16 relocations for a regular symbol.
#
# R_MIPS_HI16: (AHL + S) - (short)(AHL + S)
# R_MIPS_LO16: AHL + S
#   where AHL = (AHI << 16) + ALO

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s -t %t.exe | FileCheck %s

# CHECK: Contents of section .text:
# CHECK-NEXT: 4000a0 40000000 e1000000 40000000 df000000
# CHECK-NEXT: 4000b0 40000000 df020000 40000000 e1fe0000
# CHECK-NEXT: 4000c0 42000000 df000000 3e000000 e1000000
# CHECK-NEXT: 4000d0 40020000 df000000 40fe0000 e1000000
# CHECK-NEXT: 4000e0 00000000

# CHECK: SYMBOL TABLE:
# CHECK: 004000e0 g F .text 00000004 T2
# CHECK: 004000a0 g F .text 00000040 T0
# CHECK: 004000c0 g F .text 00000040 T1

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
- Name:         .text.1
  Type:         SHT_PROGBITS
  Content:      "000000000100000000000000ffff000000000000ff0100000000000001fe0000"
#                ^ %hi(T2+1)     ^ %hi(T2-1)     ^ %hi(T2+0x1ff) ^ %hi(T2-0x1ff)
#                        ^ %lo(T2+1)     ^ %lo(T2-1)     ^ %lo(T2+0x1ff) ^ %lo(T2-0x1ff)
  AddressAlign: 16
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]

- Name:         .rel.text.1
  Type:         SHT_REL
  Info:         .text.1
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 4
      Symbol: T2
      Type:   R_MIPS_LO16
    - Offset: 8
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 12
      Symbol: T2
      Type:   R_MIPS_LO16
    - Offset: 16
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 20
      Symbol: T2
      Type:   R_MIPS_LO16
    - Offset: 24
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 28
      Symbol: T2
      Type:   R_MIPS_LO16

- Name:         .text.2
  Type:         SHT_PROGBITS
  Content:      "02000000ffff0000feff00000100000000020000ffff000000fe000001000000"
#                ^ %hi(T2+0x1ffff)               ^ %hi(T2+0x1ffffff)
#                        ^ %lo(T2+0x1ffff)               ^ %lo(T2+0x1ffffff)
#                                ^ %hi(T2-0x1ffff)               ^ %hi(T2-0x1ffffff)
#                                        ^ %lo(T2-0x1ffff)               ^ %lo(T2-0x1ffffff)
  AddressAlign: 16
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]

- Name:         .rel.text.2
  Type:         SHT_REL
  Info:         .text.2
  AddressAlign: 4
  Relocations:
    - Offset: 0x0
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 0x4
      Symbol: T2
      Type:   R_MIPS_LO16
    - Offset: 0x8
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 0xC
      Symbol: T2
      Type:   R_MIPS_LO16
    - Offset: 0x10
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 0x14
      Symbol: T2
      Type:   R_MIPS_LO16
    - Offset: 0x18
      Symbol: T2
      Type:   R_MIPS_HI16
    - Offset: 0x1C
      Symbol: T2
      Type:   R_MIPS_LO16

- Name:         .text.3
  Type:         SHT_PROGBITS
  Size:         4
  AddressAlign: 16
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]

Symbols:
  Global:
    - Name:    T0
      Section: .text.1
      Type:    STT_FUNC
      Value:   0
      Size:    64
    - Name:    T1
      Section: .text.2
      Type:    STT_FUNC
      Value:   0
      Size:    64
    - Name:    T2
      Section: .text.3
      Type:    STT_FUNC
      Value:   0
      Size:    4
