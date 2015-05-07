# Check handling .MIPS.abiflags section with wrong version.

# RUN: yaml2obj -format=elf %s > %t.o
# RUN: not %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t.so %t.o 2>&1 \
# RUN:       | FileCheck %s

# CHECK: unexpected .MIPS.abiflags section version number '1' in {{.*}}abi-flags-01.ts.tmp

FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R6]

Sections:
- Name:         .text
  Type:         SHT_PROGBITS
  Flags:        [ SHF_ALLOC, SHF_EXECINSTR ]
  Size:         4
  AddressAlign: 16

- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  Version:      1
  ISA:          MIPS32

Symbols:
  Global:
    - Name:    T0
      Section: .text
      Type:    STT_FUNC
      Value:   0
      Size:    4
