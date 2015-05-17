# Check handling of R_MIPS_TLS_GOTTPREL and R_MIPS_TLS_GD relocations
# and generation of corresponding dynamic relocations R_MIPS_TLS_TPREL32,
# R_MIPS_TLS_DTPMOD32 and R_MIPS_TLS_DTPREL32 in case of shared library.

# Create a shared library with thread symbol D1.
# RUN: yaml2obj -format=elf -o %t-so.o %s
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so %t-so.o

# Check dynamic relocations and GOT in the shared library.
# RUN: llvm-readobj -r -dynamic-table -dt -mips-plt-got %t.so \
# RUN:   | FileCheck -check-prefix=OBJ %s
# RUN: llvm-objdump -s %t.so | FileCheck -check-prefix=RAW %s

# OBJ:      Section (4) .rel.dyn {
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_DTPMOD32 D1 0x0
# OBJ-NEXT:   0x{{[0-9A-F]+}} R_MIPS_TLS_DTPREL32 D1 0x0
# OBJ-NEXT: }

# OBJ: Name: D1@ (18)
# OBJ: Name: T1@ (21)

# OBJ: 0x7000000A MIPS_LOCAL_GOTNO 2
# OBJ: 0x70000013 MIPS_GOTSYM      0x6

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
  - Name:            .text
    Type:            SHT_PROGBITS
    Flags:           [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:    4
    Size:            4

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
...
