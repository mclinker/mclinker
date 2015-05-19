# Check that in case of an executable file linking symbol referred
# by the R_MIPS_TLS_GD relocation gets an entry in the dynamic symbol table.

# RUN: yaml2obj -format=elf -docnum 1 -o %t-so.o %s
# RUN: yaml2obj -format=elf -docnum 2 -o %t-exe.o %s
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t.so %t-so.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T0 -o %t.exe %t-exe.o %t.so

# RUN: llvm-readobj -r -dt %t.exe | FileCheck %s

# CHECK:      Relocations [
# CHECK-NEXT:   Section (5) .rel.dyn {
# CHECK-NEXT:     0x{{[0-90A-F]+}} R_MIPS_TLS_DTPMOD32 T1 0x0
# CHECK-NEXT:     0x{{[0-90A-F]+}} R_MIPS_TLS_DTPREL32 T1 0x0
# CHECK-NEXT:   }
# CHECK-NEXT: ]

# CHECK:      Symbol {
# CHECK:        Name: T1@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 4
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: TLS
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: .tdata
# CHECK-NEXT: }

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
    Size:          8

Symbols:
  Global:
    - Name:     S1
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     8

# exe.o
---
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
    Size:          8

  - Name:          .rel.text
    Type:          SHT_REL
    Link:          .symtab
    AddressAlign:  4
    Info:          .text
    Relocations:
      - Offset:  0
        Symbol:  T1
        Type:    R_MIPS_TLS_GD
      - Offset:  4
        Symbol:  S1
        Type:    R_MIPS_CALL16

  - Name:          .tdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC, SHF_TLS ]
    AddressAlign:  4
    Size:          4

Symbols:
  Global:
    - Name:     T0
      Type:     STT_FUNC
      Section:  .text
      Size:     8
    - Name:     T1
      Type:     STT_TLS
      Section:  .tdata
      Value:    0
      Size:     4
    - Name:     S1
...
