# Check that the linker shows errors if ELF header flags
# and .MIPS.abiflags section are incompatible.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-isa.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-ext.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-ases.o
# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared \
# RUN:           -o %t.so %t-isa.o %t-ext.o %t-ases.o 2>&1 \
# RUN:   | FileCheck -check-prefix=ERR %s
# RUN: llvm-readobj -mips-abi-flags %t.so | FileCheck -check-prefix=SEC %s

# ERR: inconsistent ISA between .MIPS.abiflags and ELF header e_flags field: abi-flags-08.ts.tmp-isa
# ERR: inconsistent ISA extensions between .MIPS.abiflags and ELF header e_flags field: abi-flags-08.ts.tmp-ext
# ERR: inconsistent ASEs between .MIPS.abiflags and ELF header e_flags field: abi-flags-08.ts.tmp-ases

# SEC: There is no .MIPS.abiflags section in the file.

# isa.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  2

# ext.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_3, EF_MIPS_MACH_4650]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS3
  ISAExtension: EXT_4120

# ases.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2, EF_MIPS_MICROMIPS]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  2
  ASEs:         []

...
