//===- X86RelocationFunction.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_X86_APPLY_RELOC_FUNC(Name) \
static X86RelocationFactory::Result Name    (Relocation& pEntry, \
					     const MCLDInfo& pLDInfo, \
					     X86RelocationFactory& pParent);

#define DECL_X86_APPLY_RELOC_FUNCS \
DECL_X86_APPLY_RELOC_FUNC(none)             \
DECL_X86_APPLY_RELOC_FUNC(abs32)            \
DECL_X86_APPLY_RELOC_FUNC(rel32)            \
DECL_X86_APPLY_RELOC_FUNC(plt32)            \
DECL_X86_APPLY_RELOC_FUNC(got32)            \
DECL_X86_APPLY_RELOC_FUNC(gotoff32)	    \
DECL_X86_APPLY_RELOC_FUNC(gotpc32)


#define DECL_X86_APPLY_RELOC_FUNC_PTRS \
  { &none,               0, "R_386_NONE"              },  \
  { &abs32,              1, "R_386_32"                },  \
  { &rel32,              2, "R_386_PC32"              },  \
  { &got32,              3, "R_386_GOT32"             },  \
  { &plt32,              4, "R_386_PLT32"             },  \
  { &none,               5, "R_386_COPY"              },  \
  { &none,               6, "R_386_GLOB_DAT"          },  \
  { &none,               7, "R_386_JMP_SLOT"          },  \
  { &none,               8, "R_386_RELATIVE"          },  \
  { &gotoff32,           9, "R_386_GOTOFF"            },  \
  { &gotpc32,           10, "R_386_GOTPC"             }
