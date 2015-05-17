# Check handling of R_MIPS_TLS_GOTTPREL and R_MIPS_TLS_GD relocations
# and generation of corresponding dynamic relocations R_MIPS_TLS_TPREL64,
# R_MIPS_TLS_DTPMOD64 and R_MIPS_TLS_DTPREL64 in case of shared library.

# Create a shared library with thread symbol D1.
# RUN: yaml2obj -format=elf -o %t-so.o %s
# RUN: %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so %t-so.o

# Check dynamic relocations and GOT in the shared library.
# RUN: llvm-readobj -r -dynamic-table -dt -mips-plt-got %t.so \
# RUN:   | FileCheck -check-prefix=OBJ %s
# RUN: llvm-objdump -s %t.so | FileCheck -check-prefix=RAW %s

# OBJ:      Section (4) .rel.dyn {
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_DTPMOD64/R_MIPS_NONE/R_MIPS_NONE D1 0x0
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_DTPREL64/R_MIPS_NONE/R_MIPS_NONE D1 0x0
# OBJ-NEXT: }

# OBJ: Name: D1@ (18)
# OBJ: Name: T1@ (21)

# OBJ: 0x000000007000000A MIPS_LOCAL_GOTNO 2
# OBJ: 0x0000000070000013 MIPS_GOTSYM      0x6

# OBJ:      Primary GOT {
# OBJ-NEXT:   Canonical gp value: 0x18158
# OBJ-NEXT:   Reserved entries [
# OBJ-NEXT:     Entry {
# OBJ-NEXT:       Address: 0x{{[0-9A-F]+}}
# OBJ-NEXT:       Access: -32752
# OBJ-NEXT:       Initial: 0x0
# OBJ-NEXT:       Purpose: Lazy resolver
# OBJ-NEXT:     }
# OBJ-NEXT:     Entry {
# OBJ-NEXT:       Address: 0x{{[0-9A-F]+}}
# OBJ-NEXT:       Access: -32744
# OBJ-NEXT:       Initial: 0x8000000000000000
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
# RAW-NEXT:  {{[0-9a-f]+}} 00000000 00000000 00000000 00000080
# RAW-NEXT:  {{[0-9a-f]+}} 00000000 00000000 00000000 00000000

---
FileHeader:
  Class:   ELFCLASS64
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ARCH_64]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  4
    Size:          4

  - Name:          .rel.text
    Type:          SHT_RELA
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
    Size:          8

Symbols:
  Global:
    - Name:     T1
      Type:     STT_FUNC
      Section:  .text
      Size:     4
    - Name:     D1
      Type:     STT_TLS
      Section:  .tdata
      Size:     8
...
