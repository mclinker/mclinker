# Check handling of R_MIPS_TLS_GOTTPREL and R_MIPS_TLS_GD relocations
# and generation of corresponding dynamic relocations R_MIPS_TLS_TPREL32,
# R_MIPS_TLS_DTPMOD32 and R_MIPS_TLS_DTPREL32 in case of executable linking.

# Create a shared library with thread symbol D1.
# RUN: yaml2obj -format=elf -docnum 1 -o %t-so.o %s
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so %t-so.o

# Create executable file by linking two object files and the shared library.
# The object files defines thread symbols D0 and D2.
# RUN: yaml2obj -format=elf -docnum 2 -o %t-o1.o %s
# RUN: yaml2obj -format=elf -docnum 3 -o %t-o2.o %s
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T0 -o %t.exe %t-o1.o %t-o2.o %t.so

# Check dynamic relocations and GOT in the executable file.
# RUN: llvm-readobj -r -dynamic-table -dt -mips-plt-got %t.exe \
# RUN:   | FileCheck -check-prefix=OBJ %s
# RUN: llvm-objdump -s %t.exe | FileCheck -check-prefix=RAW %s

# OBJ:      Section (5) .rel.dyn {
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_TPREL32 D1 0x0
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_TPREL32 D2 0x0
# OBJ-NEXT: }

# OBJ: Name: D1@ (34)
# OBJ: Name: D2@ (37)

# OBJ: 0x7000000A MIPS_LOCAL_GOTNO 2
# OBJ: 0x70000013 MIPS_GOTSYM      0x9

# OBJ:      Primary GOT {
# OBJ-NEXT:   Canonical gp value: 0x{{[0-9A-F]+}}
# OBJ-NEXT:   Reserved entries [
# OBJ-NEXT:     Entry {
# OBJ-NEXT:       Address: 0x{{[0-9A-F]+}}
# OBJ-NEXT:       Access: -32752
# OBJ-NEXT:       Initial: 0x0
# OBJ-NEXT:       Purpose: Lazy resolver
# OBJ-NEXT:     }
# OBJ-NEXT:     Entry {
# OBJ-NEXT:       Address: 0x{{[0-9A-F]+}}
# OBJ-NEXT:       Access: -32748
# OBJ-NEXT:       Initial: 0x80000000
# OBJ-NEXT:       Purpose: Module pointer (GNU extension)
# OBJ-NEXT:     }
# OBJ-NEXT:   ]
# OBJ-NEXT:   Local entries [
# OBJ-NEXT:   ]
# OBJ-NEXT:   Global entries [
# OBJ-NEXT:   ]
# OBJ-NEXT:   Number of TLS and multi-GOT entries: 2
# OBJ-NEXT: }

# RAW:      Contents of section .got:
# RAW-NEXT:  {{[0-9a-f]+}} 00000000 00000080 00000000 00000000

# so.o
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
    Info:          .text
    AddressAlign:  4
    Relocations:
      - Offset:  0
        Symbol:  D1
        Type:    R_MIPS_TLS_GD

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Size:          4

Symbols:
  Global:
    - Name:     T1
      Type:     STT_FUNC
      Section:  .text
      Size:     4
    - Name:     D1
      Type:     STT_TLS
      Section:  .tdata
      Size:     4

# o1.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  OSABI:    ELFOSABI_GNU
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_CPIC,
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
    Info:          .text
    AddressAlign:  4
    Relocations:
      - Offset:  0
        Symbol:  D2
        Type:    R_MIPS_TLS_TPREL_HI16
      - Offset:  4
        Symbol:  D2
        Type:    R_MIPS_TLS_TPREL_LO16

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Size:          4

Symbols:
  Global:
    - Name:     T2
      Type:     STT_FUNC
      Section:  .text
      Size:     8
    - Name:     D2
      Type:     STT_TLS
      Section:  .tdata
      Size:     4

# o2.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  OSABI:    ELFOSABI_GNU
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_CPIC,
             EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          16

  - Name:            .rel.text
    Type:            SHT_REL
    Link:            .symtab
    Info:            .text
    AddressAlign:    4
    Relocations:
      - Offset:  0
        Symbol:  D1
        Type:    R_MIPS_TLS_GOTTPREL
      - Offset:  4
        Symbol:  D0
        Type:    R_MIPS_TLS_TPREL_HI16
      - Offset:  8
        Symbol:  D0
        Type:    R_MIPS_TLS_TPREL_LO16
      - Offset:  12
        Symbol:  D2
        Type:    R_MIPS_TLS_GOTTPREL

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Size:          4

Symbols:
  Global:
    - Name:     D0
      Type:     STT_TLS
      Section:  .tdata
      Size:     4
    - Name:     T0
      Type:     STT_FUNC
      Section:  .text
      Size:     16
    - Name:     D1
      Type:     STT_TLS
    - Name:     D2
      Type:     STT_TLS
...
