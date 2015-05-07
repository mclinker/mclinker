# Check generation of .MIPS.abiflags section under the following conditions:
# - There are multiple input object files
# - Every input file has a .MIPS.abiflags section
# - .MIPS.abiflags sections have different and compatible FP ABI
# The .MIPS.abiflags section in the output file should reproduce result
# of merging FP ABI flags but the linker must show a warning.

# RUN: yaml2obj -format=elf -docnum 1 %s > %t-xx.o
# RUN: yaml2obj -format=elf -docnum 2 %s > %t-double.o
# RUN: yaml2obj -format=elf -docnum 3 %s > %t-single.o
# RUN: yaml2obj -format=elf -docnum 4 %s > %t-soft.o
# RUN: yaml2obj -format=elf -docnum 5 %s > %t-64.o
# RUN: yaml2obj -format=elf -docnum 6 %s > %t-64a.o

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t1 \
# RUN:           %t-xx.o %t-single.o 2>&1 \
# RUN:   | FileCheck -check-prefix=XX-SINGLE-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t1 | FileCheck -check-prefix=XX-SINGLE %s

# XX-SINGLE-WARN:  FP ABI -mfpxx is incompatible with -msingle-float used by
# XX-SINGLE:  FP ABI:     Hard float (32-bit CPU, Any FPU) (0x5)
# XX-SINGLE:  GPR size:   32
# XX-SINGLE:  CPR1 size:  32
# XX-SINGLE:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t2 \
# RUN:           %t-xx.o %t-soft.o 2>&1 \
# RUN:   | FileCheck -check-prefix=XX-SOFT-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t2 | FileCheck -check-prefix=XX-SOFT %s

# XX-SOFT-WARN:  FP ABI -mfpxx is incompatible with -msoft-float used by
# XX-SOFT:  FP ABI:     Hard float (32-bit CPU, Any FPU) (0x5)
# XX-SOFT:  GPR size:   32
# XX-SOFT:  CPR1 size:  32
# XX-SOFT:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t3 \
# RUN:           %t-double.o %t-single.o 2>&1 \
# RUN:   | FileCheck -check-prefix=DOUBLE-SINGLE-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t3 | FileCheck -check-prefix=DOUBLE-SINGLE %s

# DOUBLE-SINGLE-WARN:  FP ABI -mdouble-float is incompatible with -msingle-float used by
# DOUBLE-SINGLE:  FP ABI:     Hard float (double precision) (0x1)
# DOUBLE-SINGLE:  GPR size:   32
# DOUBLE-SINGLE:  CPR1 size:  32
# DOUBLE-SINGLE:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t4 \
# RUN:           %t-double.o %t-soft.o 2>&1 \
# RUN:   | FileCheck -check-prefix=DOUBLE-SOFT-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t4 | FileCheck -check-prefix=DOUBLE-SOFT %s

# DOUBLE-SOFT-WARN:  FP ABI -mdouble-float is incompatible with -msoft-float used by
# DOUBLE-SOFT:  FP ABI:     Hard float (double precision) (0x1)
# DOUBLE-SOFT:  GPR size:   32
# DOUBLE-SOFT:  CPR1 size:  32
# DOUBLE-SOFT:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t5 \
# RUN:           %t-double.o %t-64.o 2>&1 \
# RUN:   | FileCheck -check-prefix=DOUBLE-64-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t5 | FileCheck -check-prefix=DOUBLE-64 %s

# DOUBLE-64-WARN:  FP ABI -mdouble-float is incompatible with -mgp32 -mfp64 used by
# DOUBLE-64:  FP ABI:     Hard float (double precision) (0x1)
# DOUBLE-64:  GPR size:   32
# DOUBLE-64:  CPR1 size:  64
# DOUBLE-64:  Flags 1     [ (0x1)
# DOUBLE-64:    ODDSPREG (0x1)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t6 \
# RUN:           %t-double.o %t-64a.o 2>&1 \
# RUN:   | FileCheck -check-prefix=DOUBLE-64A-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t6 | FileCheck -check-prefix=DOUBLE-64A %s

# DOUBLE-64A-WARN:  FP ABI -mdouble-float is incompatible with -mgp32 -mfp64 -mno-odd-spreg used by
# DOUBLE-64A:  FP ABI:     Hard float (double precision) (0x1)
# DOUBLE-64A:  GPR size:   32
# DOUBLE-64A:  CPR1 size:  64
# DOUBLE-64A:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t7 \
# RUN:           %t-single.o %t-soft.o 2>&1 \
# RUN:   | FileCheck -check-prefix=SINGLE-SOFT-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t7 | FileCheck -check-prefix=SINGLE-SOFT %s

# SINGLE-SOFT-WARN:  FP ABI -msingle-float is incompatible with -msoft-float used by
# SINGLE-SOFT:  FP ABI:     Hard float (single precision) (0x2)
# SINGLE-SOFT:  GPR size:   32
# SINGLE-SOFT:  CPR1 size:  32
# SINGLE-SOFT:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t8 \
# RUN:           %t-single.o %t-64.o 2>&1 \
# RUN:   | FileCheck -check-prefix=SINGLE-64-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t8 | FileCheck -check-prefix=SINGLE-64 %s

# SINGLE-64-WARN:  FP ABI -msingle-float is incompatible with -mgp32 -mfp64 used by
# SINGLE-64:  FP ABI:     Hard float (single precision) (0x2)
# SINGLE-64:  GPR size:   32
# SINGLE-64:  CPR1 size:  64
# SINGLE-64:  Flags 1     [ (0x1)
# SINGLE-64:    ODDSPREG (0x1)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t9 \
# RUN:           %t-single.o %t-64a.o 2>&1 \
# RUN:   | FileCheck -check-prefix=SINGLE-64A-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t9 | FileCheck -check-prefix=SINGLE-64A %s

# SINGLE-64A-WARN:  FP ABI -msingle-float is incompatible with -mgp32 -mfp64 -mno-odd-spreg used by
# SINGLE-64A:  FP ABI:     Hard float (single precision) (0x2)
# SINGLE-64A:  GPR size:   32
# SINGLE-64A:  CPR1 size:  64
# SINGLE-64A:  Flags 1     [ (0x0)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t10 \
# RUN:           %t-soft.o %t-64.o 2>&1 \
# RUN:   | FileCheck -check-prefix=SOFT-64-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t10 | FileCheck -check-prefix=SOFT-64 %s

# SOFT-64-WARN:  FP ABI -msoft-float is incompatible with -mgp32 -mfp64 used by
# SOFT-64:  FP ABI:     Soft float (0x3)
# SOFT-64:  GPR size:   32
# SOFT-64:  CPR1 size:  64
# SOFT-64:  Flags 1     [ (0x1)
# SOFT-64:    ODDSPREG (0x1)

# RUN: %MCLinker -mtriple=mipsel-unknown-linux -shared -o %t11 \
# RUN:           %t-soft.o %t-64a.o 2>&1 \
# RUN:   | FileCheck -check-prefix=SOFT-64A-WARN %s
# RUN: llvm-readobj -mips-abi-flags %t11 | FileCheck -check-prefix=SOFT-64A %s

# SOFT-64A-WARN:  FP ABI -msoft-float is incompatible with -mgp32 -mfp64 -mno-odd-spreg used by
# SOFT-64A:  FP ABI:     Soft float (0x3)
# SOFT-64A:  GPR size:   32
# SOFT-64A:  CPR1 size:  64
# SOFT-64A:  Flags 1     [ (0x0)

# xx.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_PIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  1
  FpABI:        FP_XX
  GPRSize:      REG_32
  CPR1Size:     REG_32

# double.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_PIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  1
  FpABI:        FP_DOUBLE
  GPRSize:      REG_32
  CPR1Size:     REG_32

# single.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_PIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  1
  FpABI:        FP_SINGLE
  GPRSize:      REG_32
  CPR1Size:     REG_32

# soft.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_PIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  1
  FpABI:        FP_SOFT
  GPRSize:      REG_32
  CPR1Size:     REG_NONE

# 64.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_PIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  2
  FpABI:        FP_64
  GPRSize:      REG_32
  CPR1Size:     REG_64
  Flags1:       [ODDSPREG]

# 64a.o
---
FileHeader:
  Class:   ELFCLASS32
  Data:    ELFDATA2LSB
  Type:    ET_REL
  Machine: EM_MIPS
  Flags:   [EF_MIPS_CPIC, EF_MIPS_PIC, EF_MIPS_ABI_O32, EF_MIPS_ARCH_32R2]

Sections:
- Name: .MIPS.abiflags
  Type: SHT_MIPS_ABIFLAGS
  AddressAlign: 8
  ISA:          MIPS32
  ISARevision:  2
  FpABI:        FP_64A
  GPRSize:      REG_32
  CPR1Size:     REG_64
  Flags1:       []
...
