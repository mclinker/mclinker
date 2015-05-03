# Check handling of HI16 and LO16 relocations against _gp_disp symbol.
#
# R_MIPS_HI16: (AHL + GP - P) - (short)(AHL + GP - P)
# R_MIPS_LO16: AHL + GP - P + 4
#   where AHL = (AHI << 16) + ALO

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so %t.o
# RUN: llvm-objdump -s -t %t.so | FileCheck %s

# CHECK: Contents of section .text:
# CHECK-NEXT: 0180 01000000 017f0000 01000000 f77e0000
# CHECK-NEXT: 0190 02000000 ef800000 01000000 e97c0000
# CHECK-NEXT: 01a0 03000000 df7e0000

# CHECK: SYMBOL TABLE:
# CHECK: 00018080  g  d   *ABS*  00000000 _gp_disp
# CHECK: 00000180  g   F  .text  00000050 T0

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Content:      "000000000100000000000000ffff000000000000ff0100000000000001fe000002000000ffff0000"
#                ^ %hi(gp+1)     ^ %hi(gp-1)     ^ %hi(gp+0x1ff) ^ %hi(gp-0x1ff)         ^ %lo(gp+0x1ffff)
#                        ^ %lo(gp+1)     ^ %lo(gp-1)     ^ %lo(gp+0x1ff) ^ %lo(gp-0x1ff)
#                                                                                ^ %hi(gp+0x1ffff)
  AddressAlign: 16
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]

- Name:         .rel.text
  Type:         SHT_REL
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: _gp_disp
      Type:   R_MIPS_HI16
    - Offset: 4
      Symbol: _gp_disp
      Type:   R_MIPS_LO16
    - Offset: 8
      Symbol: _gp_disp
      Type:   R_MIPS_HI16
    - Offset: 12
      Symbol: _gp_disp
      Type:   R_MIPS_LO16
    - Offset: 16
      Symbol: _gp_disp
      Type:   R_MIPS_HI16
    - Offset: 20
      Symbol: _gp_disp
      Type:   R_MIPS_LO16
    - Offset: 24
      Symbol: _gp_disp
      Type:   R_MIPS_HI16
    - Offset: 28
      Symbol: _gp_disp
      Type:   R_MIPS_LO16

    - Offset: 32
      Symbol: _gp_disp
      Type:   R_MIPS_HI16
    - Offset: 36
      Symbol: _gp_disp
      Type:   R_MIPS_LO16

Symbols:
  Global:
    - Name:    T0
      Section: .text
      Type:    STT_FUNC
      Value:   0
      Size:    80
    - Name:    _gp_disp
