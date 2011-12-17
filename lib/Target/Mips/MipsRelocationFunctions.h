//===- MipsRelocationFunction.h -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_MIPS_APPLY_RELOC_FUNCS \
static MipsRelocationFactory::Result none   (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result abs32  (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result rel32  (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result hi16   (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result lo16   (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result got16  (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result call16 (Relocation& pEntry, const MipsRelocationFactory& pParent); \
static MipsRelocationFactory::Result gprel32(Relocation& pEntry, const MipsRelocationFactory& pParent);


#define DECL_MIPS_APPLY_RELOC_FUNC_PTRS \
  { &none,     0, "R_MIPS_NONE"    },  \
  { &none,     1, "R_MIPS_16"      },  \
  { &abs32,    2, "R_MIPS_32"      },  \
  { &rel32,    3, "R_MIPS_REL32"   },  \
  { &none,     4, "R_MIPS_26"      },  \
  { &hi16,     5, "R_MIPS_HI16"    },  \
  { &lo16,     6, "R_MIPS_LO16"    },  \
  { &none,     7, "R_MIPS_GPREL16" },  \
  { &none,     8, "R_MIPS_LITERAL" },  \
  { &got16,    9, "R_MIPS_GOT16"   },  \
  { &none,    10, "R_MIPS_PC16"    },  \
  { &call16,  11, "R_MIPS_CALL16"  },  \
  { &gprel32, 12, "R_MIPS_GPREL32" }

