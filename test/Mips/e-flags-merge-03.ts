# Check PIC/CPIC flags merging in case of multiple input objects.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-none.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-cpic.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-pic.o
# RUN: yaml2obj -format=elf -docnum 4 %s > %t-both.o

# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T1 -o %t-abi1.exe \
# RUN:     %t-none.o %t-pic.o 2>&1 | FileCheck -check-prefix=ABI-CALLS-WARN1 %s
# RUN: llvm-readobj -file-headers %t-abi1.exe \
# RUN:              | FileCheck -check-prefix=ABI-CALLS1 %s

# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T1 -o %t-abi2.exe \
# RUN:     %t-cpic.o %t-none.o 2>&1 | FileCheck -check-prefix=ABI-CALLS-WARN2 %s
# RUN: llvm-readobj -file-headers %t-abi2.exe \
# RUN:              | FileCheck -check-prefix=ABI-CALLS2 %s

# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T2 -o %t-cpic.exe \
# RUN:     %t-cpic.o %t-pic.o
# RUN: llvm-readobj -file-headers %t-cpic.exe | FileCheck -check-prefix=CPIC %s

# RUN: %MCLinker -mtriple=mipsel-linux-gnu -e T3 -o %t-both.exe \
# RUN:     %t-pic.o %t-both.o
# RUN: llvm-readobj -file-headers %t-both.exe | FileCheck -check-prefix=BOTH %s

# ABI-CALLS-WARN1: conflicting linking abicalls and non-abicalls files on e-flags-merge-03.ts.tmp-pic.
# ABI-CALLS-WARN2: conflicting linking abicalls and non-abicalls files on e-flags-merge-03.ts.tmp-none.

# ABI-CALLS1:      Flags [ (0x50001005)
# ABI-CALLS1-NEXT:   EF_MIPS_ABI_O32 (0x1000)
# ABI-CALLS1-NEXT:   EF_MIPS_ARCH_32 (0x50000000)
# ABI-CALLS1-NEXT:   EF_MIPS_CPIC (0x4)
# ABI-CALLS1-NEXT:   EF_MIPS_NOREORDER (0x1)
# ABI-CALLS1-NEXT: ]

# ABI-CALLS2:      Flags [ (0x50001005)
# ABI-CALLS2-NEXT:   EF_MIPS_ABI_O32 (0x1000)
# ABI-CALLS2-NEXT:   EF_MIPS_ARCH_32 (0x50000000)
# ABI-CALLS2-NEXT:   EF_MIPS_CPIC (0x4)
# ABI-CALLS2-NEXT:   EF_MIPS_NOREORDER (0x1)
# ABI-CALLS2-NEXT: ]

# CPIC:      Flags [ (0x50001005)
# CPIC-NEXT:   EF_MIPS_ABI_O32 (0x1000)
# CPIC-NEXT:   EF_MIPS_ARCH_32 (0x50000000)
# CPIC-NEXT:   EF_MIPS_CPIC (0x4)
# CPIC-NEXT:   EF_MIPS_NOREORDER (0x1)
# CPIC-NEXT: ]

# BOTH:      Flags [ (0x50001007)
# BOTH-NEXT:   EF_MIPS_ABI_O32 (0x1000)
# BOTH-NEXT:   EF_MIPS_ARCH_32 (0x50000000)
# BOTH-NEXT:   EF_MIPS_CPIC (0x4)
# BOTH-NEXT:   EF_MIPS_NOREORDER (0x1)
# BOTH-NEXT:   EF_MIPS_PIC (0x2)
# BOTH-NEXT: ]

# none.o
---
FileHeader:
  Class:           ELFCLASS32
  Data:            ELFDATA2LSB
  Type:            ET_REL
  Machine:         EM_MIPS
  Flags:           [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  0x04
    Size:          0x04

Symbols:
  Global:
    - Name:        T1
      Section:     .text

# cpic.o
---
FileHeader:
  Class:           ELFCLASS32
  Data:            ELFDATA2LSB
  Type:            ET_REL
  Machine:         EM_MIPS
  Flags:           [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32, EF_MIPS_CPIC]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  0x04
    Size:          0x04

Symbols:
  Global:
    - Name:        T2
      Section:     .text

# pic.o
---
FileHeader:
  Class:           ELFCLASS32
  Data:            ELFDATA2LSB
  Type:            ET_REL
  Machine:         EM_MIPS
  Flags:           [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32, EF_MIPS_PIC]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  0x04
    Size:          0x04

Symbols:
  Global:
    - Name:        T3
      Section:     .text

# both.o
---
FileHeader:
  Class:           ELFCLASS32
  Data:            ELFDATA2LSB
  Type:            ET_REL
  Machine:         EM_MIPS
  Flags:           [EF_MIPS_ABI_O32, EF_MIPS_ARCH_32, EF_MIPS_CPIC, EF_MIPS_PIC]

Sections:
  - Name:          .text
    Type:          SHT_PROGBITS
    Flags:         [ SHF_ALLOC, SHF_EXECINSTR ]
    AddressAlign:  0x04
    Size:          0x04

Symbols:
  Global:
    - Name:        T4
      Section:     .text
...
