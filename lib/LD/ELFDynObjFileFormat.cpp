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
  f_pDynSymTab    = pLinker.getOrCreateSectHdr(".dynsym",
                                               LDFileFormat::SymbolTable,
                                               ELF::SHT_DYNSYM,
                                               ELF::SHF_ALLOC);
  f_pDynStrTab    = pLinker.getOrCreateSectHdr(".dynstr",
                                               LDFileFormat::StringTable,
                                               ELF::SHT_STRTAB,
                                               ELF::SHF_ALLOC);
  f_pInterp       = pLinker.getOrCreateSectHdr(".interp",
                                               LDFileFormat::MetaData,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC);
  f_pHashTab      = pLinker.getOrCreateSectHdr(".hash",
                                               LDFileFormat::MetaData,
                                               ELF::SHT_HASH,
                                               ELF::SHF_ALLOC);
  f_pDynamic      = pLinker.getOrCreateSectHdr(".dynamic",
                                               LDFileFormat::MetaData,
                                               ELF::SHT_DYNAMIC,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pRelaDyn      = pLinker.getOrCreateSectHdr(".rela.dyn",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_RELA,
                                               ELF::SHF_ALLOC);
  f_pRelaPlt      = pLinker.getOrCreateSectHdr(".rela.plt",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_RELA,
                                               ELF::SHF_ALLOC);
  f_pRelaDyn      = pLinker.getOrCreateSectHdr(".rel.dyn",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_REL,
                                               ELF::SHF_ALLOC);
  f_pRelaPlt      = pLinker.getOrCreateSectHdr(".rel.plt",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_REL,
                                               ELF::SHF_ALLOC);
  f_pGOT          = pLinker.getOrCreateSectHdr(".got",
                                               LDFileFormat::GOT,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPLT          = pLinker.getOrCreateSectHdr(".plt",
                                               LDFileFormat::PLT,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pGOTPLT       = pLinker.getOrCreateSectHdr(".got.plt",
                                               LDFileFormat::GOT,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPreInitArray = pLinker.getOrCreateSectHdr(".preinit_array",
                                               LDFileFormat::Data,
                                               ELF::SHT_PREINIT_ARRAY,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pInitArray    = pLinker.getOrCreateSectHdr(".init_array",
                                               LDFileFormat::Data,
                                               ELF::SHT_INIT_ARRAY,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pFiniArray    = pLinker.getOrCreateSectHdr(".fini_array",
                                               LDFileFormat::Data,
                                               ELF::SHT_FINI_ARRAY,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pCtors        = pLinker.getOrCreateSectHdr(".ctors",
                                               LDFileFormat::Data,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pDtors        = pLinker.getOrCreateSectHdr(".dtors",
                                               LDFileFormat::Data,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
}

