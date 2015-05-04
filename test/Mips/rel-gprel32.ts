# Check R_MIPS_GPREL32 relocation handling in case of O32 ABI.
#
# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-unknown-linux -e T0 -o %t.exe %t.o
# RUN: llvm-objdump -s %t.exe | FileCheck -check-prefix=SEC %s
# RUN: llvm-nm %t.exe | FileCheck --check-prefix=SYM %s

# 0x08ff8087 == 0x8000001 (addend) + 0x400094 ($L1) +
#               0x0000000 (GP0) - 0x417ff0 (_gp)
# SEC:      Contents of section .rodata:
# SEC-NEXT:  4000a4 a580fe07 00000000 00000000 00000000

# SYM: 00400094 t $L1
# SYM: 00410000 d _GLOBAL_OFFSET_TABLE_

FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC,
              EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]
Sections:
- Type:         SHT_PROGBITS
  Name:         .text
  Type:         SHT_PROGBITS
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]
  AddressAlign: 4
  Size:         16

- Type:         SHT_PROGBITS
  Name:         .rodata
  Type:         SHT_PROGBITS
  Flags:        [ SHF_ALLOC ]
  AddressAlign: 4
  Content:      01000008000000000000000000000000

- Type:         SHT_REL
  Name:         .rel.rodata
  Type:         SHT_REL
  Link:         .symtab
  Info:         .rodata
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: $L1
      Type:   R_MIPS_GPREL32

- Type:         SHT_MIPS_REGINFO
  Name:         .reginfo
  Type:         SHT_MIPS_REGINFO
  Flags:        [ SHF_ALLOC ]
  AddressAlign: 0x01
# .reginfo section is unsupported 000000000000000000000000000000000000000002000001
  Size:         24

Symbols:
  Local:
    - Name:     $L1
      Section:  .text
      Value:    0x00
    - Name:     .rodata
      Type:     STT_SECTION
      Section:  .rodata
  Global:
    - Name:     T0
      Section:  .text
      Type:     STT_FUNC
      Value:    0x04
      Size:     12
    - Name:     _gp_disp
