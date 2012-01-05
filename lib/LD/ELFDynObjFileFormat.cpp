//===- ELFDynObjFileFormat.cpp --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFDynObjFileFormat.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSection.h>
#include <mcld/MC/MCLinker.h>
#include <llvm/Support/ELF.h>

using namespace mcld;
using namespace llvm;

void ELFDynObjFileFormat::initObjectType(MCLinker& pLinker)
{
  f_pDynSymTab    = &pLinker.getOrCreateOutputSectHdr(".dynsym",
                                           LDFileFormat::NamePool,
                                           ELF::SHT_DYNSYM,
                                           ELF::SHF_ALLOC);
  f_pDynStrTab    = &pLinker.getOrCreateOutputSectHdr(".dynstr",
                                           LDFileFormat::NamePool,
                                           ELF::SHT_STRTAB,
                                           ELF::SHF_ALLOC);
  f_pInterp       = &pLinker.getOrCreateOutputSectHdr(".interp",
                                           LDFileFormat::Note,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC);
  f_pHashTab      = &pLinker.getOrCreateOutputSectHdr(".hash",
                                           LDFileFormat::NamePool,
                                           ELF::SHT_HASH,
                                           ELF::SHF_ALLOC);
  f_pDynamic      = &pLinker.getOrCreateOutputSectHdr(".dynamic",
                                           LDFileFormat::Target,
                                           ELF::SHT_DYNAMIC,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pRelaDyn      = &pLinker.getOrCreateOutputSectHdr(".rela.dyn",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_RELA,
                                           ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.getOrCreateOutputSectHdr(".rela.plt",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_RELA,
                                           ELF::SHF_ALLOC);
  f_pRelaDyn      = &pLinker.getOrCreateOutputSectHdr(".rel.dyn",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_REL,
                                           ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.getOrCreateOutputSectHdr(".rel.plt",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_REL,
                                           ELF::SHF_ALLOC);
  f_pGOT          = &pLinker.getOrCreateOutputSectHdr(".got",
                                           LDFileFormat::Target,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPLT          = &pLinker.getOrCreateOutputSectHdr(".plt",
                                           LDFileFormat::Target,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pGOTPLT       = &pLinker.getOrCreateOutputSectHdr(".got.plt",
                                           LDFileFormat::Target,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pInit         = &pLinker.getOrCreateOutputSectHdr(".init",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pFini         = &pLinker.getOrCreateOutputSectHdr(".fini",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pPreInitArray = &pLinker.getOrCreateOutputSectHdr(".preinit_array",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PREINIT_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pInitArray    = &pLinker.getOrCreateOutputSectHdr(".init_array",
                                           LDFileFormat::Regular,
                                           ELF::SHT_INIT_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pFiniArray    = &pLinker.getOrCreateOutputSectHdr(".fini_array",
                                           LDFileFormat::Regular,
                                           ELF::SHT_FINI_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pCtors        = &pLinker.getOrCreateOutputSectHdr(".ctors",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pDtors        = &pLinker.getOrCreateOutputSectHdr(".dtors",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
}

