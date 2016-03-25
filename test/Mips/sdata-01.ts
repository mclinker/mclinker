# Check .sdata sections merging.

# RUN: yaml2obj -docnum=1 -format=elf %s > %t1.o
# RUN: yaml2obj -docnum=2 -format=elf %s > %t2.o
# RUN: %MCLinker -mtriple=mipsel-unknown-linux -e T0 -o %t.exe %t1.o %t2.o
# RUN: llvm-readobj -s %t.exe | FileCheck --check-prefix=SEC %s
# RUN: llvm-objdump -s %t.exe | FileCheck --check-prefix=RAW %s

# SEC:      Section {
# SEC:        Index: {{[0-9]+}}
# SEC:        Name: .sdata
# SEC-NEXT:   Type: SHT_PROGBITS (0x1)
# SEC-NEXT:   Flags [ (0x10000003)
# SEC-NEXT:     SHF_ALLOC (0x2)
# SEC-NEXT:     SHF_MIPS_GPREL (0x10000000)
# SEC-NEXT:     SHF_WRITE (0x1)
# SEC-NEXT:   ]
# SEC-NEXT:   Address: 0x410008
# SEC-NEXT:   Offset: 0x{{[0-9A-F]+}}
# SEC-NEXT:   Size: 8
# SEC-NEXT:   Link: 0
# SEC-NEXT:   Info: 0
# SEC-NEXT:   AddressAlignment: 4
# SEC-NEXT:   EntrySize: 0
# SEC-NEXT: }

# RAW:      Contents of section .sdata:
# RAW-NEXT:  410008 76543210 fedcba98

# o1.o
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
    Content:       '01234567'

  - Name:          .sdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC ]
    AddressAlign:  4
    Content:       '76543210'

Symbols:
  Local:
    - Name:     .text
      Type:     STT_SECTION
      Section:  .text
    - Name:     .sdata
      Type:     STT_SECTION
      Section:  .sdata

  Global:
    - Name:     T0
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     8
    - Name:     S0
      Type:     STT_OBJECT
      Section:  .sdata
      Value:    0
      Size:     8

# o2.o
---
FileHeader:
  Class:    ELFCLASS32
  Data:     ELFDATA2LSB
  Type:     ET_REL
  Machine:  EM_MIPS
  Flags:    [ EF_MIPS_NOREORDER, EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32 ]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  16
    Content:       '89abcdef'

  - Name:          .sdata
    Type:          SHT_PROGBITS
    Flags:         [ SHF_WRITE, SHF_ALLOC ]
    AddressAlign:  4
    Content:       'fedcba98'

Symbols:
  Local:
    - Name:     .text
      Type:     STT_SECTION
      Section:  .text
    - Name:     .sdata
      Type:     STT_SECTION
      Section:  .sdata

  Global:
    - Name:     T1
      Type:     STT_FUNC
      Section:  .text
      Value:    0
      Size:     8
    - Name:     S1
      Type:     STT_OBJECT
      Section:  .sdata
      Value:    0
      Size:     8
...
