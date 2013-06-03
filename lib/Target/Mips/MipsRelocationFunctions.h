//===- MipsRelocationFunction.h -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DECL_MIPS_APPLY_RELOC_FUNC(Name) \
static MipsRelocator::Result Name(MipsRelocationInfo& pReloc, \
                                  MipsRelocator& pParent);

#define DECL_MIPS_APPLY_RELOC_FUNCS \
DECL_MIPS_APPLY_RELOC_FUNC(none) \
DECL_MIPS_APPLY_RELOC_FUNC(abs32) \
DECL_MIPS_APPLY_RELOC_FUNC(rel26) \
DECL_MIPS_APPLY_RELOC_FUNC(hi16) \
DECL_MIPS_APPLY_RELOC_FUNC(lo16) \
DECL_MIPS_APPLY_RELOC_FUNC(gprel16) \
DECL_MIPS_APPLY_RELOC_FUNC(got16) \
DECL_MIPS_APPLY_RELOC_FUNC(call16) \
DECL_MIPS_APPLY_RELOC_FUNC(gprel32) \
DECL_MIPS_APPLY_RELOC_FUNC(abs64) \
DECL_MIPS_APPLY_RELOC_FUNC(gotdisp) \
DECL_MIPS_APPLY_RELOC_FUNC(gotoff) \
DECL_MIPS_APPLY_RELOC_FUNC(gothi16) \
DECL_MIPS_APPLY_RELOC_FUNC(gotlo16) \
DECL_MIPS_APPLY_RELOC_FUNC(sub) \
DECL_MIPS_APPLY_RELOC_FUNC(jalr) \
DECL_MIPS_APPLY_RELOC_FUNC(la25lui) \
DECL_MIPS_APPLY_RELOC_FUNC(la25j) \
DECL_MIPS_APPLY_RELOC_FUNC(la25add) \
DECL_MIPS_APPLY_RELOC_FUNC(pc32) \
DECL_MIPS_APPLY_RELOC_FUNC(unsupport)

#define DECL_MIPS_APPLY_RELOC_FUNC_PTRS \
  { &none,         0, "R_MIPS_NONE",                  0}, \
  { &unsupport,    1, "R_MIPS_16",                   16}, \
  { &abs32,        2, "R_MIPS_32",                   32}, \
  { &unsupport,    3, "R_MIPS_REL32",                32}, \
  { &rel26,        4, "R_MIPS_26",                   26}, \
  { &hi16,         5, "R_MIPS_HI16",                 16}, \
  { &lo16,         6, "R_MIPS_LO16",                 16}, \
  { &gprel16,      7, "R_MIPS_GPREL16",              16}, \
  { &unsupport,    8, "R_MIPS_LITERAL",              16}, \
  { &got16,        9, "R_MIPS_GOT16",                16}, \
  { &unsupport,   10, "R_MIPS_PC16",                 16}, \
  { &call16,      11, "R_MIPS_CALL16",               16}, \
  { &gprel32,     12, "R_MIPS_GPREL32",              32}, \
  { &none,        13, "R_MIPS_UNUSED1",               0}, \
  { &none,        14, "R_MIPS_UNUSED2",               0}, \
  { &none,        15, "R_MIPS_UNUSED3",               0}, \
  { &unsupport,   16, "R_MIPS_SHIFT5",               32}, \
  { &unsupport,   17, "R_MIPS_SHIFT6",               32}, \
  { &abs64,       18, "R_MIPS_64",                   64}, \
  { &gotdisp,     19, "R_MIPS_GOT_DISP",             16}, \
  { &gotdisp,     20, "R_MIPS_GOT_PAGE",             16}, \
  { &gotoff,      21, "R_MIPS_GOT_OFST",             16}, \
  { &gothi16,     22, "R_MIPS_GOT_HI16",             16}, \
  { &gotlo16,     23, "R_MIPS_GOT_LO16",             16}, \
  { &sub,         24, "R_MIPS_SUB",                  64}, \
  { &unsupport,   25, "R_MIPS_INSERT_A",              0}, \
  { &unsupport,   26, "R_MIPS_INSERT_B",              0}, \
  { &unsupport,   27, "R_MIPS_DELETE",                0}, \
  { &unsupport,   28, "R_MIPS_HIGHER",               16}, \
  { &unsupport,   29, "R_MIPS_HIGHEST",              16}, \
  { &gothi16,     30, "R_MIPS_CALL_HI16",            16}, \
  { &gotlo16,     31, "R_MIPS_CALL_LO16",            16}, \
  { &unsupport,   32, "R_MIPS_SCN_DISP",             32}, \
  { &unsupport,   33, "R_MIPS_REL16",                 0}, \
  { &unsupport,   34, "R_MIPS_ADD_IMMEDIATE",         0}, \
  { &unsupport,   35, "R_MIPS_PJUMP",                 0}, \
  { &unsupport,   36, "R_MIPS_RELGOT",                0}, \
  { &jalr,        37, "R_MIPS_JALR",                 32}, \
  { &unsupport,   38, "R_MIPS_TLS_DTPMOD32",         32}, \
  { &unsupport,   39, "R_MIPS_TLS_DTPREL32",         32}, \
  { &unsupport,   40, "R_MIPS_TLS_DTPMOD64",          0}, \
  { &unsupport,   41, "R_MIPS_TLS_DTPREL64",          0}, \
  { &unsupport,   42, "R_MIPS_TLS_GD",               16}, \
  { &unsupport,   43, "R_MIPS_TLS_LDM",              16}, \
  { &unsupport,   44, "R_MIPS_TLS_DTPREL_HI16",      16}, \
  { &unsupport,   45, "R_MIPS_TLS_DTPREL_LO16",      16}, \
  { &unsupport,   46, "R_MIPS_TLS_GOTTPREL",         16}, \
  { &unsupport,   47, "R_MIPS_TLS_TPREL32",          32}, \
  { &unsupport,   48, "R_MIPS_TLS_TPREL64",           0}, \
  { &unsupport,   49, "R_MIPS_TLS_TPREL_HI16",       16}, \
  { &unsupport,   50, "R_MIPS_TLS_TPREL_LO16",       16}, \
  { &unsupport,   51, "R_MIPS_GLOB_DAT",              0}, \
  { &unsupport,   52, "",                             0}, \
  { &unsupport,   53, "",                             0}, \
  { &unsupport,   54, "",                             0}, \
  { &unsupport,   55, "",                             0}, \
  { &unsupport,   56, "",                             0}, \
  { &unsupport,   57, "",                             0}, \
  { &unsupport,   58, "",                             0}, \
  { &unsupport,   59, "",                             0}, \
  { &unsupport,   60, "",                             0}, \
  { &unsupport,   61, "",                             0}, \
  { &unsupport,   62, "",                             0}, \
  { &unsupport,   63, "",                             0}, \
  { &unsupport,   64, "",                             0}, \
  { &unsupport,   65, "",                             0}, \
  { &unsupport,   66, "",                             0}, \
  { &unsupport,   67, "",                             0}, \
  { &unsupport,   68, "",                             0}, \
  { &unsupport,   69, "",                             0}, \
  { &unsupport,   70, "",                             0}, \
  { &unsupport,   71, "",                             0}, \
  { &unsupport,   72, "",                             0}, \
  { &unsupport,   73, "",                             0}, \
  { &unsupport,   74, "",                             0}, \
  { &unsupport,   75, "",                             0}, \
  { &unsupport,   76, "",                             0}, \
  { &unsupport,   77, "",                             0}, \
  { &unsupport,   78, "",                             0}, \
  { &unsupport,   79, "",                             0}, \
  { &unsupport,   80, "",                             0}, \
  { &unsupport,   81, "",                             0}, \
  { &unsupport,   82, "",                             0}, \
  { &unsupport,   83, "",                             0}, \
  { &unsupport,   84, "",                             0}, \
  { &unsupport,   85, "",                             0}, \
  { &unsupport,   86, "",                             0}, \
  { &unsupport,   87, "",                             0}, \
  { &unsupport,   88, "",                             0}, \
  { &unsupport,   89, "",                             0}, \
  { &unsupport,   90, "",                             0}, \
  { &unsupport,   91, "",                             0}, \
  { &unsupport,   92, "",                             0}, \
  { &unsupport,   93, "",                             0}, \
  { &unsupport,   94, "",                             0}, \
  { &unsupport,   95, "",                             0}, \
  { &unsupport,   96, "",                             0}, \
  { &unsupport,   97, "",                             0}, \
  { &unsupport,   98, "",                             0}, \
  { &unsupport,   99, "",                             0}, \
  { &unsupport,  100, "R_MIPS16_26",                  0}, \
  { &unsupport,  101, "R_MIPS16_GPREL",               0}, \
  { &unsupport,  102, "R_MIPS16_GOT16",               0}, \
  { &unsupport,  103, "R_MIPS16_CALL16",              0}, \
  { &unsupport,  104, "R_MIPS16_HI16",                0}, \
  { &unsupport,  105, "R_MIPS16_LO16",                0}, \
  { &unsupport,  106, "R_MIPS16_TLS_GD",              0}, \
  { &unsupport,  107, "R_MIPS16_TLS_LDM",             0}, \
  { &unsupport,  108, "R_MIPS16_TLS_DTPREL_HI16",     0}, \
  { &unsupport,  109, "R_MIPS16_TLS_DTPREL_LO16",     0}, \
  { &unsupport,  110, "R_MIPS16_TLS_GOTTPREL",        0}, \
  { &unsupport,  111, "R_MIPS16_TLS_TPREL_HI16",      0}, \
  { &unsupport,  112, "R_MIPS16_TLS_TPREL_LO16",      0}, \
  { &unsupport,  113, "",                             0}, \
  { &unsupport,  114, "",                             0}, \
  { &unsupport,  115, "",                             0}, \
  { &unsupport,  116, "",                             0}, \
  { &unsupport,  117, "",                             0}, \
  { &unsupport,  118, "",                             0}, \
  { &unsupport,  119, "",                             0}, \
  { &unsupport,  120, "",                             0}, \
  { &unsupport,  121, "",                             0}, \
  { &unsupport,  122, "",                             0}, \
  { &unsupport,  123, "",                             0}, \
  { &unsupport,  124, "",                             0}, \
  { &unsupport,  125, "",                             0}, \
  { &unsupport,  126, "R_MIPS_COPY",                  0}, \
  { &unsupport,  127, "R_MIPS_JUMP_SLOT",             0}, \
  { &unsupport,  128, "",                             0}, \
  { &unsupport,  129, "",                             0}, \
  { &unsupport,  130, "",                             0}, \
  { &unsupport,  131, "",                             0}, \
  { &unsupport,  132, "",                             0}, \
  { &unsupport,  133, "R_MICROMIPS_26_S1",            0}, \
  { &unsupport,  134, "R_MICROMIPS_HI16",             0}, \
  { &unsupport,  135, "R_MICROMIPS_LO16",             0}, \
  { &unsupport,  136, "R_MICROMIPS_GPREL16",          0}, \
  { &unsupport,  137, "R_MICROMIPS_LITERAL",          0}, \
  { &unsupport,  138, "R_MICROMIPS_GOT16",            0}, \
  { &unsupport,  139, "R_MICROMIPS_PC7_S1",           0}, \
  { &unsupport,  140, "R_MICROMIPS_PC10_S1",          0}, \
  { &unsupport,  141, "R_MICROMIPS_PC16_S1",          0}, \
  { &unsupport,  142, "R_MICROMIPS_CALL16",           0}, \
  { &unsupport,  143, "R_MICROMIPS_GOT_DISP",         0}, \
  { &unsupport,  144, "R_MICROMIPS_GOT_PAGE",         0}, \
  { &unsupport,  145, "R_MICROMIPS_GOT_OFST",         0}, \
  { &unsupport,  146, "R_MICROMIPS_GOT_HI16",         0}, \
  { &unsupport,  147, "R_MICROMIPS_GOT_LO16",         0}, \
  { &unsupport,  148, "R_MICROMIPS_SUB",              0}, \
  { &unsupport,  149, "R_MICROMIPS_HIGHER",           0}, \
  { &unsupport,  150, "R_MICROMIPS_HIGHEST",          0}, \
  { &unsupport,  151, "R_MICROMIPS_CALL_HI16",        0}, \
  { &unsupport,  152, "R_MICROMIPS_CALL_LO16",        0}, \
  { &unsupport,  153, "R_MICROMIPS_SCN_DISP",         0}, \
  { &unsupport,  154, "R_MICROMIPS_JALR",             0}, \
  { &unsupport,  155, "R_MICROMIPS_HI0_LO16",         0}, \
  { &unsupport,  156, "",                             0}, \
  { &unsupport,  157, "",                             0}, \
  { &unsupport,  158, "",                             0}, \
  { &unsupport,  159, "",                             0}, \
  { &unsupport,  160, "",                             0}, \
  { &unsupport,  161, "",                             0}, \
  { &unsupport,  162, "R_MICROMIPS_TLS_GD",           0}, \
  { &unsupport,  163, "R_MICROMIPS_TLS_LDM",          0}, \
  { &unsupport,  164, "R_MICROMIPS_TLS_DTPREL_HI16",  0}, \
  { &unsupport,  165, "R_MICROMIPS_TLS_DTPREL_LO16",  0}, \
  { &unsupport,  166, "R_MICROMIPS_TLS_GOTTPREL",     0}, \
  { &unsupport,  167, "",                             0}, \
  { &unsupport,  168, "",                             0}, \
  { &unsupport,  169, "R_MICROMIPS_TLS_TPREL_HI16",   0}, \
  { &unsupport,  170, "R_MICROMIPS_TLS_TPREL_LO16",   0}, \
  { &unsupport,  171, "",                             0}, \
  { &unsupport,  172, "R_MICROMIPS_GPREL7_S2",        0}, \
  { &unsupport,  173, "R_MICROMIPS_PC23_S2",          0}, \
  { &unsupport,  174, "",                             0}, \
  { &unsupport,  175, "",                             0}, \
  { &unsupport,  176, "",                             0}, \
  { &unsupport,  177, "",                             0}, \
  { &unsupport,  178, "",                             0}, \
  { &unsupport,  179, "",                             0}, \
  { &unsupport,  180, "",                             0}, \
  { &unsupport,  181, "",                             0}, \
  { &unsupport,  182, "",                             0}, \
  { &unsupport,  183, "",                             0}, \
  { &unsupport,  184, "",                             0}, \
  { &unsupport,  185, "",                             0}, \
  { &unsupport,  186, "",                             0}, \
  { &unsupport,  187, "",                             0}, \
  { &unsupport,  188, "",                             0}, \
  { &unsupport,  189, "",                             0}, \
  { &unsupport,  190, "",                             0}, \
  { &unsupport,  191, "",                             0}, \
  { &unsupport,  192, "",                             0}, \
  { &unsupport,  193, "",                             0}, \
  { &unsupport,  194, "",                             0}, \
  { &unsupport,  195, "",                             0}, \
  { &unsupport,  196, "",                             0}, \
  { &unsupport,  197, "",                             0}, \
  { &unsupport,  198, "",                             0}, \
  { &unsupport,  199, "",                             0}, \
  { &la25lui,    200, "R_MIPS_LA25_LUI",             16}, \
  { &la25j,      201, "R_MIPS_LA25_J",               26}, \
  { &la25add,    202, "R_MIPS_LA25_ADD",             16}, \
  { &unsupport,  203, "",                             0}, \
  { &unsupport,  204, "",                             0}, \
  { &unsupport,  205, "",                             0}, \
  { &unsupport,  206, "",                             0}, \
  { &unsupport,  207, "",                             0}, \
  { &unsupport,  208, "",                             0}, \
  { &unsupport,  209, "",                             0}, \
  { &unsupport,  210, "",                             0}, \
  { &unsupport,  211, "",                             0}, \
  { &unsupport,  212, "",                             0}, \
  { &unsupport,  213, "",                             0}, \
  { &unsupport,  214, "",                             0}, \
  { &unsupport,  215, "",                             0}, \
  { &unsupport,  216, "",                             0}, \
  { &unsupport,  217, "",                             0}, \
  { &unsupport,  218, "",                             0}, \
  { &unsupport,  219, "",                             0}, \
  { &unsupport,  220, "",                             0}, \
  { &unsupport,  221, "",                             0}, \
  { &unsupport,  222, "",                             0}, \
  { &unsupport,  223, "",                             0}, \
  { &unsupport,  224, "",                             0}, \
  { &unsupport,  225, "",                             0}, \
  { &unsupport,  226, "",                             0}, \
  { &unsupport,  227, "",                             0}, \
  { &unsupport,  228, "",                             0}, \
  { &unsupport,  229, "",                             0}, \
  { &unsupport,  230, "",                             0}, \
  { &unsupport,  231, "",                             0}, \
  { &unsupport,  232, "",                             0}, \
  { &unsupport,  233, "",                             0}, \
  { &unsupport,  234, "",                             0}, \
  { &unsupport,  235, "",                             0}, \
  { &unsupport,  236, "",                             0}, \
  { &unsupport,  237, "",                             0}, \
  { &unsupport,  238, "",                             0}, \
  { &unsupport,  239, "",                             0}, \
  { &unsupport,  240, "",                             0}, \
  { &unsupport,  241, "",                             0}, \
  { &unsupport,  242, "",                             0}, \
  { &unsupport,  243, "",                             0}, \
  { &unsupport,  244, "",                             0}, \
  { &unsupport,  245, "",                             0}, \
  { &unsupport,  246, "",                             0}, \
  { &unsupport,  247, "",                             0}, \
  { &pc32,       248, "R_MIPS_PC32",                  0}, \
  { &unsupport,  249, "",                             0}, \
  { &unsupport,  250, "R_MIPS_GNU_REL16_S2",          0}, \
  { &unsupport,  251, "",                             0}, \
  { &unsupport,  252, "",                             0}, \
  { &unsupport,  253, "R_MIPS_GNU_VTINHERIT",         0}, \
  { &unsupport,  254, "R_MIPS_GNU_VTENTRY",           0}
