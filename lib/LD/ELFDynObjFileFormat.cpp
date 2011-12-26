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
  f_pDynSymTab    = &pLinker.createSectHdr(".dynsym",
                                           LDFileFormat::NamePool,
                                           ELF::SHT_DYNSYM,
                                           ELF::SHF_ALLOC);
  f_pDynStrTab    = &pLinker.createSectHdr(".dynstr",
                                           LDFileFormat::NamePool,
                                           ELF::SHT_STRTAB,
                                           ELF::SHF_ALLOC);
  f_pInterp       = &pLinker.createSectHdr(".interp",
                                           LDFileFormat::Note,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC);
  f_pHashTab      = &pLinker.createSectHdr(".hash",
                                           LDFileFormat::NamePool,
                                           ELF::SHT_HASH,
                                           ELF::SHF_ALLOC);
  f_pDynamic      = &pLinker.createSectHdr(".dynamic",
                                           LDFileFormat::Note,
                                           ELF::SHT_DYNAMIC,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pRelaDyn      = &pLinker.createSectHdr(".rela.dyn",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_RELA,
                                           ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.createSectHdr(".rela.plt",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_RELA,
                                           ELF::SHF_ALLOC);
  f_pRelaDyn      = &pLinker.createSectHdr(".rel.dyn",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_REL,
                                           ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.createSectHdr(".rel.plt",
                                           LDFileFormat::Relocation,
                                           ELF::SHT_REL,
                                           ELF::SHF_ALLOC);
  f_pGOT          = &pLinker.createSectHdr(".got",
                                           LDFileFormat::Target,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPLT          = &pLinker.createSectHdr(".plt",
                                           LDFileFormat::Target,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pGOTPLT       = &pLinker.createSectHdr(".got.plt",
                                           LDFileFormat::Target,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPreInitArray = &pLinker.createSectHdr(".preinit_array",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PREINIT_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pInitArray    = &pLinker.createSectHdr(".init_array",
                                           LDFileFormat::Regular,
                                           ELF::SHT_INIT_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pFiniArray    = &pLinker.createSectHdr(".fini_array",
                                           LDFileFormat::Regular,
                                           ELF::SHT_FINI_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pCtors        = &pLinker.createSectHdr(".ctors",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pDtors        = &pLinker.createSectHdr(".dtors",
                                           LDFileFormat::Regular,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
}

