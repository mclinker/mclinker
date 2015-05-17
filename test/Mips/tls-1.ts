# Check handling of R_MIPS_TLS_TPREL_HI16 / R_MIPS_TLS_TPREL_LO16 relocations.

# RUN: yaml2obj -format=elf -o %t.o %s
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e L0 -o %t.exe %t.o
# RUN: llvm-objdump -s %t.exe | FileCheck %s

# CHECK: Contents of section .text:
# CHECK:  {{[0-9A-F]+}} 00000000 01000000 03900000 04900000

FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC,
             EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Content:       '01000000020000000300000004000000'

  - Name:          .rel.text
    Type:          SHT_REL
    Info:          .text
    AddressAlign:  4
    Relocations:
      - Offset:  0
        Symbol:  L1
        Type:    R_MIPS_TLS_TPREL_HI16
      - Offset:  4
        Symbol:  L2
        Type:    R_MIPS_TLS_TPREL_HI16
      - Offset:  8
        Symbol:  L2
        Type:    R_MIPS_TLS_TPREL_LO16
      - Offset:  12
        Symbol:  L1
        Type:    R_MIPS_TLS_TPREL_LO16

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Address:       0x1000
    Size:          0x20000

Symbols:
  Global:
    - Name:     L0
      Type:     STT_FUNC
      Section:  .text
      Size:     0x58
    - Name:     L1
      Type:     STT_TLS
      Section:  .tdata
      Value:    0
      Size:     4
    - Name:     L2
      Type:     STT_TLS
      Section:  .tdata
      Value:    0x10000
      Size:     4
