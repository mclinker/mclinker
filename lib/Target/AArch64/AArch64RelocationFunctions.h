//===- AArch64RelocationFunction.h ----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// FIXME: llvm::ELF doesn't define AArch64 dynamic relocation types
enum {
  R_AARCH64_COPY         = 1024,
  R_AARCH64_GLOB_DAT     = 1025,
  R_AARCH64_JUMP_SLOT    = 1026,
  R_AARCH64_RELATIVE     = 1027,
  R_AARCH64_TLS_DTPREL64 = 1028,
  R_AARCH64_TLS_DTPMOD64 = 1029,
  R_AARCH64_TLS_TPREL64  = 1030,
  R_AARCH64_TLSDESC      = 1031,
  R_AARCH64_IRELATIVE    = 1032
};

#define DECL_AARCH64_APPLY_RELOC_FUNC(Name)                  \
static AArch64Relocator::Result Name    (Relocation& pEntry, \
                                         AArch64Relocator& pParent);

#define DECL_AARCH64_APPLY_RELOC_FUNCS          \
DECL_AARCH64_APPLY_RELOC_FUNC(none)             \
DECL_AARCH64_APPLY_RELOC_FUNC(rel)             \
DECL_AARCH64_APPLY_RELOC_FUNC(unsupport)

#define DECL_AARCH64_APPLY_RELOC_FUNC_PTRS \
  { &none,           0x100, "R_AARCH64_NONE"                               },  \
  { &unsupport,      0x101, "R_AARCH64_ABS64"                              },  \
  { &unsupport,      0x102, "R_AARCH64_ABS32"                              },  \
  { &unsupport,      0x103, "R_AARCH64_ABS16"                              },  \
  { &unsupport,      0x104, "R_AARCH64_PREL64"                             },  \
  { &rel,            0x105, "R_AARCH64_PREL32"                             },  \
  { &unsupport,      0x106, "R_AARCH64_PREL16"                             },  \
  { &unsupport,      0x107, "R_AARCH64_MOVW_UABS_G0"                       },  \
  { &unsupport,      0x108, "R_AARCH64_MOVW_UABS_G0_NC"                    },  \
  { &unsupport,      0x109, "R_AARCH64_MOVW_UABS_G1"                       },  \
  { &unsupport,      0x10a, "R_AARCH64_MOVW_UABS_G1_NC"                    },  \
  { &unsupport,      0x10b, "R_AARCH64_MOVW_UABS_G2"                       },  \
  { &unsupport,      0x10c, "R_AARCH64_MOVW_UABS_G2_NC"                    },  \
  { &unsupport,      0x10d, "R_AARCH64_MOVW_UABS_G3"                       },  \
  { &unsupport,      0x10e, "R_AARCH64_MOVW_SABS_G0"                       },  \
  { &unsupport,      0x10f, "R_AARCH64_MOVW_SABS_G1"                       },  \
  { &unsupport,      0x110, "R_AARCH64_MOVW_SABS_G2"                       },  \
  { &unsupport,      0x111, "R_AARCH64_LD_PREL_LO19"                       },  \
  { &unsupport,      0x112, "R_AARCH64_ADR_PREL_LO21"                      },  \
  { &unsupport,      0x113, "R_AARCH64_ADR_PREL_PG_HI21"                   },  \
  { &unsupport,      0x115, "R_AARCH64_ADD_ABS_LO12_NC"                    },  \
  { &unsupport,      0x116, "R_AARCH64_LDST8_ABS_LO12_NC"                  },  \
  { &unsupport,      0x117, "R_AARCH64_TSTBR14"                            },  \
  { &unsupport,      0x118, "R_AARCH64_CONDBR19"                           },  \
  { &unsupport,      0x11a, "R_AARCH64_JUMP26"                             },  \
  { &unsupport,      0x11b, "R_AARCH64_CALL26"                             },  \
  { &unsupport,      0x11c, "R_AARCH64_LDST16_ABS_LO12_NC"                 },  \
  { &unsupport,      0x11d, "R_AARCH64_LDST32_ABS_LO12_NC"                 },  \
  { &unsupport,      0x11e, "R_AARCH64_LDST64_ABS_LO12_NC"                 },  \
  { &unsupport,      0x12b, "R_AARCH64_LDST128_ABS_LO12_NC"                },  \
  { &unsupport,      0x137, "R_AARCH64_ADR_GOT_PAGE"                       },  \
  { &unsupport,      0x138, "R_AARCH64_LD64_GOT_LO12_NC"                   },  \
  { &unsupport,      0x20b, "R_AARCH64_TLSLD_MOVW_DTPREL_G2"               },  \
  { &unsupport,      0x20c, "R_AARCH64_TLSLD_MOVW_DTPREL_G1"               },  \
  { &unsupport,      0x20d, "R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC"            },  \
  { &unsupport,      0x20e, "R_AARCH64_TLSLD_MOVW_DTPREL_G0"               },  \
  { &unsupport,      0x20f, "R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC"            },  \
  { &unsupport,      0x210, "R_AARCH64_TLSLD_ADD_DTPREL_HI12"              },  \
  { &unsupport,      0x211, "R_AARCH64_TLSLD_ADD_DTPREL_LO12"              },  \
  { &unsupport,      0x212, "R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC"           },  \
  { &unsupport,      0x213, "R_AARCH64_TLSLD_LDST8_DTPREL_LO12"            },  \
  { &unsupport,      0x214, "R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC"         },  \
  { &unsupport,      0x215, "R_AARCH64_TLSLD_LDST16_DTPREL_LO12"           },  \
  { &unsupport,      0x216, "R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC"        },  \
  { &unsupport,      0x217, "R_AARCH64_TLSLD_LDST32_DTPREL_LO12"           },  \
  { &unsupport,      0x218, "R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC"        },  \
  { &unsupport,      0x219, "R_AARCH64_TLSLD_LDST64_DTPREL_LO12"           },  \
  { &unsupport,      0x21a, "R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC"        },  \
  { &unsupport,      0x21b, "R_AARCH64_TLSIE_MOVW_GOTTPREL_G1"             },  \
  { &unsupport,      0x21c, "R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC"          },  \
  { &unsupport,      0x21d, "R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21"          },  \
  { &unsupport,      0x21e, "R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC"        },  \
  { &unsupport,      0x21f, "R_AARCH64_TLSIE_LD_GOTTPREL_PREL19"           },  \
  { &unsupport,      0x220, "R_AARCH64_TLSLE_MOVW_TPREL_G2"                },  \
  { &unsupport,      0x221, "R_AARCH64_TLSLE_MOVW_TPREL_G1"                },  \
  { &unsupport,      0x222, "R_AARCH64_TLSLE_MOVW_TPREL_G1_NC"             },  \
  { &unsupport,      0x223, "R_AARCH64_TLSLE_MOVW_TPREL_G0"                },  \
  { &unsupport,      0x224, "R_AARCH64_TLSLE_MOVW_TPREL_G0_NC"             },  \
  { &unsupport,      0x225, "R_AARCH64_TLSLE_ADD_TPREL_HI12"               },  \
  { &unsupport,      0x226, "R_AARCH64_TLSLE_ADD_TPREL_LO12"               },  \
  { &unsupport,      0x227, "R_AARCH64_TLSLE_ADD_TPREL_LO12_NC"            },  \
  { &unsupport,      0x228, "R_AARCH64_TLSLE_LDST8_TPREL_LO12"             },  \
  { &unsupport,      0x229, "R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC"          },  \
  { &unsupport,      0x22a, "R_AARCH64_TLSLE_LDST16_TPREL_LO12"            },  \
  { &unsupport,      0x22b, "R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC"         },  \
  { &unsupport,      0x22c, "R_AARCH64_TLSLE_LDST32_TPREL_LO12"            },  \
  { &unsupport,      0x22d, "R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC"         },  \
  { &unsupport,      0x22e, "R_AARCH64_TLSLE_LDST64_TPREL_LO12"            },  \
  { &unsupport,      0x22f, "R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC"         },  \
  { &unsupport,      0x232, "R_AARCH64_TLSDESC_ADR_PAGE"                   },  \
  { &unsupport,      0x233, "R_AARCH64_TLSDESC_LD64_LO12_NC"               },  \
  { &unsupport,      0x234, "R_AARCH64_TLSDESC_ADD_LO12_NC"                },  \
  { &unsupport,      0x239, "R_AARCH64_TLSDESC_CALL"                       },  \
  { &unsupport,       1024,  "R_AARCH64_COPY"                             },  \
  { &unsupport,       1025,  "R_AARCH64_GLOB_DAT"                         },  \
  { &unsupport,       1026,  "R_AARCH64_JUMP_SLOT"                        },  \
  { &unsupport,       1027,  "R_AARCH64_RELATIVE"                         },  \
  { &unsupport,       1028,  "R_AARCH64_TLS_DTPREL64"                     },  \
  { &unsupport,       1029,  "R_AARCH64_TLS_DTPMOD64"                     },  \
  { &unsupport,       1030,  "R_AARCH64_TLS_TPREL64"                      },  \
  { &unsupport,       1031,  "R_AARCH64_TLSDESC"                          },  \
  { &unsupport,       1032,  "R_AARCH64_IRELATIVE"                        }
