# Check that if no input object has .sdata section, the output
# file will not have it too.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: %MCLinker -mtriple=mipsel-unknown-linux -e T0 -o %t.exe %t.o
# RUN: llvm-readobj -s %t.exe | FileCheck %s

# CHECK-NOT:  Name: .sdata

---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [EF_MIPS_NOREORDER, EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Size:          8

Symbols:
  Local:
    - Name:     .text
      Type:     STT_SECTION
      Section:  .text

  Global:
    - Name:     T0
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     8
