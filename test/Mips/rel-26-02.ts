# Check reading addendum for R_MIPS_26 relocation and writing result back.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -t -s %t.exe | FileCheck %s

# CHECK:      Contents of section .text:
# CHECK-NEXT:  4000a0 2d001002 2d001000 27001002 27001000
#                     ^ (A: 0x7fffffc + S: 0x4000b8) >> 2 = 0x210002d
#                              ^ (A: -4 + S: 0x4000b8) >> 2 = 0x10002d
# ((A: 0x7fffffc | ((P: 0x400068 + 4) & 0x3f000000))
#       + S: 0x4000a0) >> 2 = 0x2100027 ^
#          ((A: -4 | ((P: 0x40006c + 4) & 0x3f000000))
#                + S: 0x4000a0) >> 2 = 0x100027  ^
# CHECK-NEXT:  4000b0 00000000 00000000 4000193c 2c001008
# CHECK-NEXT:  4000c0 b0003927 00000000

# CHECK: SYMBOL TABLE:
# CHECK: 004000a0 l  d  .text  00000000 .text
# CHECK: 004000b8 l   F .text  00000010 __T1_pic@island-0
# CHECK: 004000a0       .text  00000010 T0
# CHECK: 004000b0       .text  00000004 T1

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Content:  "ffffff01ffffff03ffffff01ffffff030000000000000000"
#            ^ A: 0x1ffffff  ^ A: 0x1ffffff
#                    ^ A: -4         ^ A: -4
  AddressAlign: 16
  Flags:        [SHF_EXECINSTR, SHF_ALLOC]

- Name:         .rel.text
  Type:         SHT_REL
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T1
      Type:   R_MIPS_26
    - Offset: 4
      Symbol: T1
      Type:   R_MIPS_26
    - Offset: 8
      Symbol: .text
      Type:   R_MIPS_26
    - Offset: 12
      Symbol: .text
      Type:   R_MIPS_26

Symbols:
  Local:
    - Name:    .text
      Type:    STT_SECTION
      Section: .text
  Global:
    - Name:    T0
      Section: .text
      Value:   0
      Size:    16
    - Name:    T1
      Section: .text
      Value:   16
      Size:    4
