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

void ELFDynObjFileFormat::initObjectType(MCLinker& pLinker)
{
  f_pDynSymTab    = &pLinker.getOrCreateOutputSectHdr(".dynsym",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_DYNSYM,
                                           llvm::ELF::SHF_ALLOC);
  f_pDynStrTab    = &pLinker.getOrCreateOutputSectHdr(".dynstr",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_STRTAB,
                                           llvm::ELF::SHF_ALLOC);
  f_pInterp       = &pLinker.getOrCreateOutputSectHdr(".interp",
                                           LDFileFormat::Note,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC);
  f_pHashTab      = &pLinker.getOrCreateOutputSectHdr(".hash",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_HASH,
                                           llvm::ELF::SHF_ALLOC);
  f_pDynamic      = &pLinker.getOrCreateOutputSectHdr(".dynamic",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_DYNAMIC,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pRelaDyn      = &pLinker.getOrCreateOutputSectHdr(".rela.dyn",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_RELA,
                                           llvm::ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.getOrCreateOutputSectHdr(".rela.plt",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_RELA,
                                           llvm::ELF::SHF_ALLOC);
  f_pRelDyn      = &pLinker.getOrCreateOutputSectHdr(".rel.dyn",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_REL,
                                           llvm::ELF::SHF_ALLOC);
  f_pRelPlt      = &pLinker.getOrCreateOutputSectHdr(".rel.plt",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_REL,
                                           llvm::ELF::SHF_ALLOC);
  f_pGOT          = &pLinker.getOrCreateOutputSectHdr(".got",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pPLT          = &pLinker.getOrCreateOutputSectHdr(".plt",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR);
  f_pGOTPLT       = &pLinker.getOrCreateOutputSectHdr(".got.plt",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pInit         = &pLinker.getOrCreateOutputSectHdr(".init",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR);
  f_pFini         = &pLinker.getOrCreateOutputSectHdr(".fini",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR);
  f_pPreInitArray = &pLinker.getOrCreateOutputSectHdr(".preinit_array",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_PREINIT_ARRAY,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pInitArray    = &pLinker.getOrCreateOutputSectHdr(".init_array",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_INIT_ARRAY,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pFiniArray    = &pLinker.getOrCreateOutputSectHdr(".fini_array",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_FINI_ARRAY,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pCtors        = &pLinker.getOrCreateOutputSectHdr(".ctors",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pDtors        = &pLinker.getOrCreateOutputSectHdr(".dtors",
                                           LDFileFormat::Regular,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
}

