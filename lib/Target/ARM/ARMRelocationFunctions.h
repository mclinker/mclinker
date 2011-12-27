//===- ARMRelocationFunction.def ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_ARM_APPLY_RELOC_FUNC(Name) \
static ARMRelocationFactory::Result Name    (Relocation& pEntry, ARMRelocationFactory& pParent);

#define DECL_ARM_APPLY_RELOC_FUNCS \
DECL_ARM_APPLY_RELOC_FUNC(none)     \
DECL_ARM_APPLY_RELOC_FUNC(abs32)    \
DECL_ARM_APPLY_RELOC_FUNC(rel32)    \
DECL_ARM_APPLY_RELOC_FUNC(gotoff32) \
DECL_ARM_APPLY_RELOC_FUNC(gotbrel)  \
DECL_ARM_APPLY_RELOC_FUNC(plt32)    \
DECL_ARM_APPLY_RELOC_FUNC(jump24)


#define DECL_ARM_APPLY_RELOC_FUNC_PTRS \
  { &none,       0, "R_ARM_NONE"              },  \
  { &none,       1, "R_ARM_PC24"              },  \
  { &abs32,      2, "R_ARM_ABS32"             },  \
  { &rel32,      3, "R_ARM_REL32"             },  \
  { &none,       4, "R_ARM_LDR_PC_G0"         },  \
  { &none,       5, "R_ARM_ABS16"             },  \
  { &none,       6, "R_ARM_ABS12"             },  \
  { &none,       7, "R_ARM_THM_ABS5"          },  \
  { &none,       8, "R_ARM_ABS8"              },  \
  { &none,       9, "R_ARM_SBREL32"           },  \
  { &none,      10, "R_ARM_THM_CALL"          },  \
  { &none,      11, "R_ARM_THM_PC8"           },  \
  { &none,      12, "R_ARM_BREL_ADJ"          },  \
  { &none,      13, "R_ARM_TLS_DESC"          },  \
  { &none,      14, "R_ARM_THM_SWI8"          },  \
  { &none,      15, "R_ARM_XPC25"             },  \
  { &none,      16, "R_ARM_THM_XPC22"         },  \
  { &none,      17, "R_ARM_TLS_DTPMOD32"      },  \
  { &none,      18, "R_ARM_TLS_DTPOFF32"      },  \
  { &none,      19, "R_ARM_TLS_TPOFF32"       },  \
  { &none,      20, "R_ARM_COPY"              },  \
  { &none,      21, "R_ARM_GLOB_DAT"          },  \
  { &none,      22, "R_ARM_JUMP_SLOT"         },  \
  { &none,      23, "R_ARM_RELATIVE"          },  \
  { &gotoff32,  24, "R_ARM_GOTOFF32"          },  \
  { &none,      25, "R_ARM_BASE_PREL"         },  \
  { &gotbrel,   26, "R_ARM_GOT_BREL"          },  \
  { &plt32,     27, "R_ARM_PLT32"             },  \
  { &none,      28, "R_ARM_CALL"              },  \
  { &jump24,    29, "R_ARM_JUMP24"            },  \
  { &none,      30, "R_ARM_THM_JUMP24"        },  \
  { &none,      31, "R_ARM_BASE_ABS"          },  \
  { &none,      32, "R_ARM_ALU_PCREL_7_0"     },  \
  { &none,      33, "R_ARM_ALU_PCREL_15_8"    },  \
  { &none,      34, "R_ARM_ALU_PCREL_23_15"   },  \
  { &none,      35, "R_ARM_LDR_SBREL_11_0_NC" },  \
  { &none,      36, "R_ARM_ALU_SBREL_19_12_NC"},  \
  { &none,      37, "R_ARM_ALU_SBREL_27_20_CK"},  \
  { &none,      38, "R_ARM_TARGET1"           },  \
  { &none,      39, "R_ARM_SBREL31"           },  \
  { &none,      40, "R_ARM_V4BX"              },  \
  { &none,      41, "R_ARM_TARGET2"           },  \
  { &none,      42, "R_ARM_PREL31"            },  \
  { &none,      43, "R_ARM_MOVW_ABS_NC"       },  \
  { &none,      44, "R_ARM_MOVT_ABS"          },  \
  { &none,      45, "R_ARM_MOVW_PREL_NC"      },  \
  { &none,      46, "R_ARM_MOVT_PREL"         },  \
  { &none,      47, "R_ARM_THM_MOVW_ABS_NC"   },  \
  { &none,      48, "R_ARM_THM_MOVT_ARBS"     },  \
  { &none,      49, "R_ARM_THM_MOVW_PREL_NC"  },  \
  { &none,      50, "R_ARM_THM_MOVT_PREL"     },  \
  { &none,      51, "R_ARM_THM_JUMP19"        },  \
  { &none,      52, "R_ARM_THM_JUMP6"         },  \
  { &none,      53, "R_ARM_THM_ALU_PREL_11_0" },  \
  { &none,      54, "R_ARM_THM_PC12"          },  \
  { &none,      55, "R_ARM_ABS32_NOI"         },  \
  { &none,      56, "R_ARM_REL32_NOI"         },  \
  { &none,      57, "R_ARM_ALU_PC_G0_NC"      },  \
  { &none,      58, "R_ARM_ALU_PC_G0"         },  \
  { &none,      59, "R_ARM_ALU_PC_G1_NC"      },  \
  { &none,      60, "R_ARM_ALU_PC_G1"         },  \
  { &none,      61, "R_ARM_ALU_PC_G2"         },  \
  { &none,      62, "R_ARM_LDR_PC_G1"         },  \
  { &none,      63, "R_ARM_LDR_PC_G2"         },  \
  { &none,      64, "R_ARM_LDRS_PC_G0"        },  \
  { &none,      65, "R_ARM_LDRS_PC_G1"        },  \
  { &none,      66, "R_ARM_LDRS_PC_G2"        },  \
  { &none,      67, "R_ARM_LDC_PC_G0"         },  \
  { &none,      68, "R_ARM_LDC_PC_G1"         },  \
  { &none,      69, "R_ARM_LDC_PC_G2"         },  \
  { &none,      70, "R_ARM_ALU_SB_G0_NC"      },  \
  { &none,      71, "R_ARM_ALU_SB_G0"         },  \
  { &none,      72, "R_ARM_ALU_SB_G1_NC"      },  \
  { &none,      73, "R_ARM_ALU_SB_G1"         },  \
  { &none,      74, "R_ARM_ALU_SB_G2"         },  \
  { &none,      75, "R_ARM_LDR_SB_G0"         },  \
  { &none,      76, "R_ARM_LDR_SB_G1"         },  \
  { &none,      77, "R_ARM_LDR_SB_G2"         },  \
  { &none,      78, "R_ARM_LDRS_SB_G0"        },  \
  { &none,      79, "R_ARM_LDRS_SB_G1"        },  \
  { &none,      80, "R_ARM_LDRS_SB_G2"        },  \
  { &none,      81, "R_ARM_LDC_SB_G0"         },  \
  { &none,      82, "R_ARM_LDC_SB_G1"         },  \
  { &none,      83, "R_ARM_LDC_SB_G2"         },  \
  { &none,      84, "R_ARM_MOVW_BREL_NC"      },  \
  { &none,      85, "R_ARM_MOVT_BREL"         },  \
  { &none,      86, "R_ARM_MOVW_BREL"         },  \
  { &none,      87, "R_ARM_THM_MOVW_BREL_NC"  },  \
  { &none,      88, "R_ARM_THM_MOVT_BREL"     },  \
  { &none,      89, "R_ARM_THM_MOVW_BREL"     },  \
  { &none,      90, "R_ARM_TLS_GOTDESC"       },  \
  { &none,      91, "R_ARM_TLS_CALL"          },  \
  { &none,      92, "R_ARM_TLS_DESCSEQ"       },  \
  { &none,      93, "R_ARM_THM_TLS_CALL"      },  \
  { &none,      94, "R_ARM_PLT32_ABS"         },  \
  { &none,      95, "R_ARM_GOT_ABS"           },  \
  { &none,      96, "R_ARM_GOT_PREL"          },  \
  { &none,      97, "R_ARM_GOT_PREL12"        },  \
  { &none,      98, "R_ARM_GOTOFF12"          },  \
  { &none,      99, "R_ARM_GOTRELAX"          },  \
  { &none,     100, "R_ARM_GNU_VTENTRY"       },  \
  { &none,     101, "R_ARM_GNU_VTINERIT"      },  \
  { &none,     102, "R_ARM_THM_JUMP11"        },  \
  { &none,     103, "R_ARM_THM_JUMP8"         },  \
  { &none,     104, "R_ARM_TLS_GD32"          },  \
  { &none,     105, "R_ARM_TLS_LDM32"         },  \
  { &none,     106, "R_ARM_TLS_LDO32"         },  \
  { &none,     107, "R_ARM_TLS_IE32"          },  \
  { &none,     108, "R_ARM_TLS_LE32"          },  \
  { &none,     109, "R_ARM_TLS_LDO12"         },  \
  { &none,     110, "R_ARM_TLS_LE12"          },  \
  { &none,     111, "R_ARM_TLS_IE12GP"        },  \
  { &none,     112, "R_ARM_PRIVATE_0"         },  \
  { &none,     113, "R_ARM_PRIVATE_1"         },  \
  { &none,     114, "R_ARM_PRIVATE_2"         },  \
  { &none,     115, "R_ARM_PRIVATE_3"         },  \
  { &none,     116, "R_ARM_PRIVATE_4"         },  \
  { &none,     117, "R_ARM_PRIVATE_5"         },  \
  { &none,     118, "R_ARM_PRIVATE_6"         },  \
  { &none,     119, "R_ARM_PRIVATE_7"         },  \
  { &none,     120, "R_ARM_PRIVATE_8"         },  \
  { &none,     121, "R_ARM_PRIVATE_9"         },  \
  { &none,     122, "R_ARM_PRIVATE_10"        },  \
  { &none,     123, "R_ARM_PRIVATE_11"        },  \
  { &none,     124, "R_ARM_PRIVATE_12"        },  \
  { &none,     125, "R_ARM_PRIVATE_13"        },  \
  { &none,     126, "R_ARM_PRIVATE_14"        },  \
  { &none,     127, "R_ARM_PRIVATE_15"        },  \
  { &none,     128, "R_ARM_ME_TOO"            },  \
  { &none,     129, "R_ARM_THM_TLS_DESCSEQ16" },  \
  { &none,     130, "R_ARM_THM_TLS_DESCSEQ32" }

