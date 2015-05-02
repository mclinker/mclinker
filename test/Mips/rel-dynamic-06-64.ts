# Conditions:
#   a) Linking a shared library.
#   b) The first relocation modifies a regular .text section.
#   c) The second relocation modifies a .pdr section without SHF_ALLOC flag.
# Check:
#   a) There should be no PLT entries.
#   b) Linker creates a single R_MIPS_REL32 relocation.
#
# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mips64el-linux-gnu -shared -o %t.so %t.o
# RUN: llvm-objdump -s %t.so | FileCheck -check-prefix=RAW %s
# RUN: llvm-readobj -dt -r -s %t.so | FileCheck -check-prefix=SYM %s

# RAW:      Contents of section .text:
# RAW-NEXT:  0240 00000000 40020000

# SYM:     Sections [
# SYM:       Section {
# SYM-NOT:     Name: .plt ({{[0-9]+}})

# SYM:      Relocations [
# SYM-NEXT:   Section (4) .rel.dyn {
# SYM-NEXT:     0x[[ADDR1:[0-9A-Z]+]] R_MIPS_REL32/R_MIPS_64/R_MIPS_NONE - 0x0
# SYM-NEXT:     0x[[ADDR2:[0-9A-Z]+]] R_MIPS_REL32/R_MIPS_64/R_MIPS_NONE T0 0x0
# SYM-NEXT:   }
# SYM-NEXT: ]

# SYM:      Symbol {
# SYM:        Name: @
# SYM-NEXT:   Value: 0x0
# SYM-NEXT:   Size: 0
# SYM-NEXT:   Binding: Local
# SYM-NEXT:   Type: None
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: Undefined
# SYM-NEXT: }
# SYM:      Symbol {
# SYM:        Name: T0@
# SYM-NEXT:   Value: 0x[[ADDR2]]
# SYM-NEXT:   Size: 4
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: .text
# SYM-NEXT: }
# SYM:      Symbol {
# SYM:        Name: T1@
# SYM-NEXT:   Value: 0x[[ADDR1]]
# SYM-NEXT:   Size: 4
# SYM-NEXT:   Binding: Global
# SYM-NEXT:   Type: Function
# SYM-NEXT:   Other: 0
# SYM-NEXT:   Section: .text
# SYM-NEXT: }

FileHeader:
  Class:   ELFCLASS64
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_NOREORDER, EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ARCH_64]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Size:         8
  AddressAlign: 16
  Flags:        [SHF_EXECINSTR, SHF_ALLOC]

- Name:         .rel.text
  Type:         SHT_RELA
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T0
      Type:   R_MIPS_64
    - Offset: 4
      Symbol: .text
      Type:   R_MIPS_64

- Name:         .pdr
  Type:         SHT_PROGBITS
  Size:         8
  AddressAlign: 16

- Name:         .rel.pdr
  Type:         SHT_RELA
  Info:         .pdr
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: T1
      Type:   R_MIPS_64

Symbols:
  Local:
    - Name:    .text
      Type:    STT_SECTION
      Section: .text
  Global:
    - Name:    T0
      Section: .text
      Type:    STT_FUNC
      Value:   0x0
      Size:    4
    - Name:    T1
      Section: .text
      Type:    STT_FUNC
      Value:   4
      Size:    4
