# Check handling of R_MIPS_TLS_LDM relocation and generation of corresponding
# dynamic relocation R_MIPS_TLS_DTPMOD32.

# RUN: yaml2obj -format=elf -docnum 1 -o %t-so1.o %s
# RUN: yaml2obj -format=elf -docnum 2 -o %t-so2.o %s
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so %t-so1.o %t-so2.o

# RUN: llvm-readobj -r -dynamic-table -dt -mips-plt-got %t.so \
# RUN:   | FileCheck -check-prefix=OBJ %s
# RUN: llvm-objdump -s %t.so | FileCheck -check-prefix=RAW %s

# OBJ:      Section (4) .rel.dyn {
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_DTPMOD32 - 0x0
# OBJ-NEXT: }

# OBJ: Name: @ (0)
# OBJ: Name: T1@ (25)
# OBJ: Name: T2@ (28)
# OBJ: Name: T3@ (31)

# OBJ: 0x7000000A MIPS_LOCAL_GOTNO 2
# OBJ: 0x70000013 MIPS_GOTSYM      0x7

# RAW:      Contents of section .got:
# RAW-NEXT:  {{[0-9a-f]+}} 00000000 00000080 00000000 00000000
#                          Two LDM entries --^--------^

# so1.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  OSABI:    ELFOSABI_GNU
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC,
             EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          8

  - Name:          .rel.text
    Type:          SHT_REL
    Link:          .symtab
    AddressAlign:  4
    Info:          .text
    Relocations:
      - Offset:  0
        Symbol:  L01
        Type:    R_MIPS_TLS_LDM
      - Offset:  4
        Symbol:  L01
        Type:    R_MIPS_TLS_LDM

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Size:          4

Symbols:
  Local:
    - Name:     L01
      Type:     STT_TLS
      Section:  .tdata
      Size:     4
  Global:
    - Name:     T1
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     4
    - Name:     T2
      Type:     STT_FUNC
      Section:  .text
      Value:    4
      Size:     4

# so2.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  OSABI:    ELFOSABI_GNU
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC,
             EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

  - Name:          .rel.text
    Type:          SHT_REL
    Link:          .symtab
    AddressAlign:  4
    Info:          .text
    Relocations:
      - Offset:  0
        Symbol:  L02
        Type:    R_MIPS_TLS_LDM

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Size:          4

Symbols:
  Local:
    - Name:     L02
      Type:     STT_TLS
      Section:  .tdata
      Size:     4
  Global:
    - Name:     T3
      Type:     STT_FUNC
      Section:  .text
      Size:     4
...
