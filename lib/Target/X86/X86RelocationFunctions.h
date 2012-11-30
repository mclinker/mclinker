//===- X86RelocationFunction.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_X86_APPLY_RELOC_FUNC(Name) \
static X86Relocator::Result Name    (Relocation& pEntry, \
					                          X86Relocator& pParent);

#define DECL_X86_APPLY_RELOC_FUNCS \
DECL_X86_APPLY_RELOC_FUNC(none)             \
DECL_X86_APPLY_RELOC_FUNC(abs)              \
DECL_X86_APPLY_RELOC_FUNC(rel)              \
DECL_X86_APPLY_RELOC_FUNC(plt32)            \
DECL_X86_APPLY_RELOC_FUNC(got32)            \
DECL_X86_APPLY_RELOC_FUNC(gotoff32)	        \
DECL_X86_APPLY_RELOC_FUNC(gotpc32)          \
DECL_X86_APPLY_RELOC_FUNC(tls_gd)           \
DECL_X86_APPLY_RELOC_FUNC(tls_ie)           \
DECL_X86_APPLY_RELOC_FUNC(tls_gotie)        \
DECL_X86_APPLY_RELOC_FUNC(tls_le)           \
DECL_X86_APPLY_RELOC_FUNC(tls_ldm)          \
DECL_X86_APPLY_RELOC_FUNC(tls_ldo_32)       \
DECL_X86_APPLY_RELOC_FUNC(unsupport)


#define DECL_X86_APPLY_RELOC_FUNC_PTRS \
  { &none,               0, "R_386_NONE"              },  \
  { &abs,                1, "R_386_32"                },  \
  { &rel,                2, "R_386_PC32"              },  \
  { &got32,              3, "R_386_GOT32"             },  \
  { &plt32,              4, "R_386_PLT32"             },  \
  { &none,               5, "R_386_COPY"              },  \
  { &none,               6, "R_386_GLOB_DAT"          },  \
  { &none,               7, "R_386_JMP_SLOT"          },  \
  { &none,               8, "R_386_RELATIVE"          },  \
  { &gotoff32,           9, "R_386_GOTOFF"            },  \
  { &gotpc32,           10, "R_386_GOTPC"             },  \
  { &unsupport,         11, "R_386_32PLT"             },  \
  { &unsupport,         12, ""                        },  \
  { &unsupport,         13, ""                        },  \
  { &unsupport,         14, "R_386_TLS_TPOFF"         },  \
  { &tls_ie,            15, "R_386_TLS_IE"            },  \
  { &tls_gotie,         16, "R_386_TLS_GOTIE"         },  \
  { &tls_le,            17, "R_386_TLS_LE"            },  \
  { &tls_gd,            18, "R_386_TLS_GD"            },  \
  { &tls_ldm,           19, "R_386_TLS_LDM"           },  \
  { &abs,               20, "R_386_16"                },  \
  { &rel,               21, "R_386_PC16"              },  \
  { &abs,               22, "R_386_8"                 },  \
  { &rel,               23, "R_386_PC8"               },  \
  { &unsupport,         24, "R_386_TLS_GD_32"         },  \
  { &unsupport,         25, "R_386_TLS_GD_PUSH"       },  \
  { &unsupport,         26, "R_386_TLS_GD_CALL"       },  \
  { &unsupport,         27, "R_386_TLS_GD_POP"        },  \
  { &unsupport,         28, "R_386_TLS_LDM_32"        },  \
  { &unsupport,         29, "R_386_TLS_LDM_PUSH"      },  \
  { &unsupport,         30, "R_386_TLS_LDM_CALL"      },  \
  { &unsupport,         31, "R_386_TLS_LDM_POP"       },  \
  { &tls_ldo_32,        32, "R_386_TLS_LDO_32"        },  \
  { &unsupport,         33, "R_386_TLS_IE_32"         },  \
  { &unsupport,         34, "R_386_TLS_LE_32"         },  \
  { &unsupport,         35, "R_386_TLS_DTPMOD32"      },  \
  { &unsupport,         36, "R_386_TLS_DTPOFF32"      },  \
  { &unsupport,         37, "R_386_TLS_TPOFF32"       },  \
  { &unsupport,         38, ""                        },  \
  { &unsupport,         39, "R_386_TLS_GOTDESC"       },  \
  { &unsupport,         40, "R_386_TLS_DESC_CALL"     },  \
  { &unsupport,         41, "R_386_TLS_DESC"          },  \
  { &unsupport,         42, "R_386_IRELATIVE"         },  \
  { &unsupport,         43,  "R_386_NUM"              }
