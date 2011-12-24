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
                                           LDFileFormat::SymbolTable,
                                           ELF::SHT_DYNSYM,
                                           ELF::SHF_ALLOC);
  f_pDynStrTab    = &pLinker.createSectHdr(".dynstr",
                                           LDFileFormat::StringTable,
                                           ELF::SHT_STRTAB,
                                           ELF::SHF_ALLOC);
  f_pInterp       = &pLinker.createSectHdr(".interp",
                                           LDFileFormat::MetaData,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC);
  f_pHashTab      = &pLinker.createSectHdr(".hash",
                                           LDFileFormat::MetaData,
                                           ELF::SHT_HASH,
                                           ELF::SHF_ALLOC);
  f_pDynamic      = &pLinker.createSectHdr(".dynamic",
                                           LDFileFormat::MetaData,
                                           ELF::SHT_DYNAMIC,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pRelaDyn      = &pLinker.createSectHdr(".rela.dyn",
                                           LDFileFormat::ReadOnly,
                                           ELF::SHT_RELA,
                                           ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.createSectHdr(".rela.plt",
                                           LDFileFormat::ReadOnly,
                                           ELF::SHT_RELA,
                                           ELF::SHF_ALLOC);
  f_pRelaDyn      = &pLinker.createSectHdr(".rel.dyn",
                                           LDFileFormat::ReadOnly,
                                           ELF::SHT_REL,
                                           ELF::SHF_ALLOC);
  f_pRelaPlt      = &pLinker.createSectHdr(".rel.plt",
                                           LDFileFormat::ReadOnly,
                                           ELF::SHT_REL,
                                           ELF::SHF_ALLOC);
  f_pGOT          = &pLinker.createSectHdr(".got",
                                           LDFileFormat::GOT,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPLT          = &pLinker.createSectHdr(".plt",
                                           LDFileFormat::PLT,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pGOTPLT       = &pLinker.createSectHdr(".got.plt",
                                           LDFileFormat::GOT,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPreInitArray = &pLinker.createSectHdr(".preinit_array",
                                           LDFileFormat::Data,
                                           ELF::SHT_PREINIT_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pInitArray    = &pLinker.createSectHdr(".init_array",
                                           LDFileFormat::Data,
                                           ELF::SHT_INIT_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pFiniArray    = &pLinker.createSectHdr(".fini_array",
                                           LDFileFormat::Data,
                                           ELF::SHT_FINI_ARRAY,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pCtors        = &pLinker.createSectHdr(".ctors",
                                           LDFileFormat::Data,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pDtors        = &pLinker.createSectHdr(".dtors",
                                           LDFileFormat::Data,
                                           ELF::SHT_PROGBITS,
                                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
}

