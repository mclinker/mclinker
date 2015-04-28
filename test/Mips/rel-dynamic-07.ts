# Conditions:
#   a) Linking a shared library.
#   b) There ars multiple R_MIPS_32 relocations with various targets.
# Check:
#   a) Emitting of R_MIPS_REL32 relocations.
#   b) There should be no R_MIPS_REL32 relocations for the _gp_disp symbol.
#
# RUN: yaml2obj -format=elf -docnum 1 %s > %t-so.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t1.so %t-so.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-o.o
# RUN: %MCLinker -mtriple=mipsel-linux-gnu -shared -o %t2.so %t-o.o %t1.so
# RUN: llvm-readobj -dt -t -r -s %t2.so | FileCheck %s

# CHECK:   Sections [
# CHECK:     Section {
# CHECK-NOT:   Name: .plt

# CHECK:      Relocations [
# CHECK-NEXT:   Section (4) .rel.dyn {
# CHECK-NEXT:     0x[[ADDR1:[0-9A-Z]+]] R_MIPS_REL32 D2 0x0
# CHECK-NEXT:     0x[[ADDR2:[0-9A-Z]+]] R_MIPS_REL32 D4 0x0
# CHECK-NEXT:     0x[[ADDR1]]           R_MIPS_REL32 T0 0x0
# CHECK-NEXT:     0x[[ADDR2]]           R_MIPS_REL32 T1 0x0
# CHECK-NEXT:     0x[[ADDR3:[0-9A-Z]+]] R_MIPS_REL32 T2 0x0
# CHECK-NEXT:     0x[[ADDR1]]           R_MIPS_REL32 T4 0x0
# CHECK-NEXT:     0x[[ADDR3]]           R_MIPS_REL32 U1 0x0
# CHECK-NEXT:     0x[[ADDR2]]           R_MIPS_REL32 D0 0x0
# CHECK-NEXT:     0x[[ADDR3]]           R_MIPS_REL32 D1 0x0
# CHECK-NEXT:   }
# CHECK-NEXT: ]

# CHECK:    Symbols [
# CHECK:      Symbol {
# CHECK:        Name: LD0
# CHECK-NEXT:   Value: 0x[[ADDR1]]

# CHECK:    DynamicSymbols [
# CHECK:      Symbol {
# CHECK:        Name: @
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Local
# CHECK-NEXT:   Type: None
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: D2@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Object
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: D4@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Object
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: T0@
# CHECK-NEXT:   Value: 0x2A4
# CHECK-NEXT:   Size: 8
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Function
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: .text
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: T1@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Function
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: T2@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Function
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: T4@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Function
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: U1@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: None
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: D0@
# CHECK-NEXT:   Value: 0x[[ADDR2]]
# CHECK-NEXT:   Size: 8
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Object
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: .data
# CHECK-NEXT: }
# CHECK:      Symbol {
# CHECK:        Name: D1@
# CHECK-NEXT:   Value: 0x0
# CHECK-NEXT:   Size: 0
# CHECK-NEXT:   Binding: Global
# CHECK-NEXT:   Type: Object
# CHECK-NEXT:   Other: 0
# CHECK-NEXT:   Section: Undefined
# CHECK-NEXT: }

# so.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Size:         12
  AddressAlign: 16
  Flags:        [SHF_EXECINSTR, SHF_ALLOC]

- Name:         .data
  Type:         SHT_PROGBITS
  Size:         8
  AddressAlign: 16
  Flags:        [SHF_WRITE, SHF_ALLOC]

Symbols:
  Global:
    - Name:    T1
      Section: .text
      Type:    STT_FUNC
      Value:   0
      Size:    4
    - Name:    T2
      Section: .text
      Type:    STT_FUNC
      Value:   4
      Size:    4
    - Name:    D1
      Section: .data
      Type:    STT_OBJECT
      Value:   0
      Size:    4
    - Name:    D2
      Section: .data
      Type:    STT_OBJECT
      Value:   4
      Size:    4

# o.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_PIC, EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Size:         12
  AddressAlign: 16
  Flags:        [SHF_EXECINSTR, SHF_ALLOC]

- Name:         .rel.text
  Type:         SHT_REL
  Info:         .text
  AddressAlign: 4
  Relocations:
    - Offset: 0
      Symbol: _gp_disp
      Type:   R_MIPS_HI16
    - Offset: 0
      Symbol: _gp_disp
      Type:   R_MIPS_LO16

- Name:         .data
  Type:         SHT_PROGBITS
  Size:         12
  AddressAlign: 16
  Flags:        [SHF_WRITE, SHF_ALLOC]

- Name:         .rel.data
  Type:         SHT_REL
  Info:         .data
  AddressAlign: 4
  Relocations:
    - Offset: 0      # T0 is a defined function
      Symbol: T0
      Type:   R_MIPS_32
    - Offset: 4      # T1 is a function from shared lib
      Symbol: T1
      Type:   R_MIPS_32
    - Offset: 8      # T2 has unknown type and defined in shared lib
      Symbol: T2
      Type:   R_MIPS_32
    - Offset: 0      # T4 is an undefined function
      Symbol: T4
      Type:   R_MIPS_32
    - Offset: 4      # D0 is a defined data object
      Symbol: D0
      Type:   R_MIPS_32
    - Offset: 8      # D1 is a data object from shared lib
      Symbol: D1
      Type:   R_MIPS_32
    - Offset: 0      # D2 has unknown type and defined in shared lib
      Symbol: D2
      Type:   R_MIPS_32
    - Offset: 4      # D4 is an undefined data object
      Symbol: D4
      Type:   R_MIPS_32
    - Offset: 8      # U1 is undefined and has unknown type
      Symbol: U1
      Type:   R_MIPS_32

Symbols:
  Local:
    - Name:    LT0
      Section: .text
      Type:    STT_FUNC
      Value:   0
      Size:    4
    - Name:    LD0
      Section: .data
      Type:    STT_OBJECT
      Value:   0
      Size:    4

  Global:
    - Name: _gp_disp
      Type: STT_OBJECT

    - Name:    T0
      Section: .text
      Type:    STT_FUNC
      Value:   4
      Size:    8
    - Name:    T1
      Type:    STT_FUNC
    - Name:    T2
    - Name:    T4
      Type:    STT_FUNC

    - Name:    D0
      Section: .data
      Type:    STT_OBJECT
      Value:   4
      Size:    8
    - Name:    D1
      Type:    STT_OBJECT
    - Name:    D2
    - Name:    D4
      Type:    STT_OBJECT
    - Name:    U1
