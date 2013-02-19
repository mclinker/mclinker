//===- X86RelocationFunction.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_X86_32_APPLY_RELOC_FUNC(Name) \
static X86Relocator::Result Name(Relocation& pEntry, X86_32Relocator& pParent);

#define DECL_X86_32_APPLY_RELOC_FUNCS \
DECL_X86_32_APPLY_RELOC_FUNC(none)             \
DECL_X86_32_APPLY_RELOC_FUNC(abs)              \
DECL_X86_32_APPLY_RELOC_FUNC(rel)              \
DECL_X86_32_APPLY_RELOC_FUNC(plt32)            \
DECL_X86_32_APPLY_RELOC_FUNC(got32)            \
DECL_X86_32_APPLY_RELOC_FUNC(gotoff32)         \
DECL_X86_32_APPLY_RELOC_FUNC(gotpc32)          \
DECL_X86_32_APPLY_RELOC_FUNC(tls_gd)           \
DECL_X86_32_APPLY_RELOC_FUNC(tls_ie)           \
DECL_X86_32_APPLY_RELOC_FUNC(tls_gotie)        \
DECL_X86_32_APPLY_RELOC_FUNC(tls_le)           \
DECL_X86_32_APPLY_RELOC_FUNC(tls_ldm)          \
DECL_X86_32_APPLY_RELOC_FUNC(tls_ldo_32)       \
DECL_X86_32_APPLY_RELOC_FUNC(unsupport)


#define DECL_X86_32_APPLY_RELOC_FUNC_PTRS \
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
  { &unsupport,         43, "R_386_NUM"               },  \
  { &none,              44, "R_386_TLS_OPT"           }

#define DECL_X86_64_APPLY_RELOC_FUNC(Name) \
static X86Relocator::Result Name(Relocation& pEntry, X86_64Relocator& pParent);

#define DECL_X86_64_APPLY_RELOC_FUNCS \
DECL_X86_64_APPLY_RELOC_FUNC(none)             \
DECL_X86_64_APPLY_RELOC_FUNC(gotpcrel)         \
DECL_X86_64_APPLY_RELOC_FUNC(plt32)            \
DECL_X86_64_APPLY_RELOC_FUNC(unsupport)

#define DECL_X86_64_APPLY_RELOC_FUNC_PTRS \
  { &none,               0, "R_X86_64_NONE",            32 },  \
  { &unsupport,          1, "R_X86_64_32",              32 },  \
  { &unsupport,          2, "R_X86_64_PC32",            32 },  \
  { &unsupport,          3, "R_X86_64_GOT32",           32 },  \
  { &plt32,              4, "R_X86_64_PLT32",           32 },  \
  { &none,               5, "R_X86_64_COPY",            32 },  \
  { &none,               6, "R_X86_64_GLOB_DAT",        32 },  \
  { &none,               7, "R_X86_64_JMP_SLOT",        32 },  \
  { &none,               8, "R_X86_64_RELATIVE",        32 },  \
  { &gotpcrel,           9, "R_X86_64_GOTPCREL",        32 },  \
  { &unsupport,         10, "R_X86_64_32",              32 },  \
  { &unsupport,         11, "R_X86_64_32S",             32 },  \
  { &unsupport,         12, "R_X86_64_16",              32 },  \
  { &unsupport,         13, "R_X86_64_PC16",            32 },  \
  { &unsupport,         14, "R_X86_64_8",               32 },  \
  { &unsupport,         15, "R_X86_64_PC8",             32 },  \
  { &none,              16, "R_X86_64_DTPMOD64",        32 },  \
  { &unsupport,         17, "R_X86_64_DTPOFF64",        32 },  \
  { &unsupport,         18, "R_X86_64_TPOFF64",         32 },  \
  { &unsupport,         19, "R_X86_64_TLSGD",           32 },  \
  { &unsupport,         20, "R_X86_64_TLSLD",           32 },  \
  { &unsupport,         21, "R_X86_64_DTPOFF32",        32 },  \
  { &unsupport,         22, "R_X86_64_GOTTPOFF",        32 },  \
  { &unsupport,         23, "R_X86_64_TPOFF32",         32 },  \
  { &unsupport,         24, "R_X86_64_PC64",            32 },  \
  { &unsupport,         25, "R_X86_64_GOTOFF64",        32 },  \
  { &unsupport,         26, "R_X86_64_GOTPC32",         32 },  \
  { &unsupport,         27, "R_X86_64_GOT64",           32 },  \
  { &unsupport,         28, "R_X86_64_GOTPCREL64",      32 },  \
  { &unsupport,         29, "R_X86_64_GOTPC64",         32 },  \
  { &unsupport,         30, "R_X86_64_GOTPLT64",        32 },  \
  { &unsupport,         31, "R_X86_64_PLTOFF64",        32 },  \
  { &unsupport,         32, "R_X86_64_SIZE32",          32 },  \
  { &unsupport,         33, "R_X86_64_SIZE64",          32 },  \
  { &unsupport,         34, "R_X86_64_GOTPC32_TLSDESC", 32 },  \
  { &unsupport,         35, "R_X86_64_TLSDESC_CALL",    32 },  \
  { &none,              36, "R_X86_64_TLSDESC",         32 },  \
  { &none,              37, "R_X86_64_IRELATIVE",       32 },  \
  { &none,              38, "R_X86_64_RELATIVE64",      32 }
