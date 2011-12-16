//===- MipsRelocationFunction.h -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_MIPS_APPLY_RELOC_FUNCS \
static MipsRelocationFactory::Result none(Relocation& pEntry, const MipsRelocationFactory& pParent);


#define DECL_MIPS_APPLY_RELOC_FUNC_PTRS \
  { &none,  0, "R_MIPS_NONE"    },  \
  { &none,  1, "R_MIPS_16"      },  \
  { &none,  2, "R_MIPS_32"      },  \
  { &none,  3, "R_MIPS_REL32"   },  \
  { &none,  4, "R_MIPS_26"      },  \
  { &none,  5, "R_MIPS_HI16"    },  \
  { &none,  6, "R_MIPS_LO16"    },  \
  { &none,  7, "R_MIPS_GPREL16" },  \
  { &none,  8, "R_MIPS_LITERAL" },  \
  { &none,  9, "R_MIPS_GOT16"   },  \
  { &none, 10, "R_MIPS_PC16"    },  \
  { &none, 11, "R_MIPS_CALL16"  },  \
  { &none, 12, "R_MIPS_GPREL32" }

