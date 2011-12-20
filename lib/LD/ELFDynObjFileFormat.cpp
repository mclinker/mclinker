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
  f_pDynSymTab    = pLinker.getOrCreateSection(".dynsym",
                                               LDFileFormat::SymbolTable,
                                               ELF::SHT_DYNSYM,
                                               ELF::SHF_ALLOC);
  f_pDynStrTab    = pLinker.getOrCreateSection(".dynstr",
                                               LDFileFormat::StringTable,
                                               ELF::SHT_STRTAB,
                                               ELF::SHF_ALLOC);
  f_pInterp       = pLinker.getOrCreateSection(".interp",
                                               LDFileFormat::MetaData,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC);
  f_pHashTab      = pLinker.getOrCreateSection(".hash",
                                               LDFileFormat::MetaData,
                                               ELF::SHT_HASH,
                                               ELF::SHF_ALLOC);
  f_pDynamic      = pLinker.getOrCreateSection(".dynamic",
                                               LDFileFormat::MetaData,
                                               ELF::SHT_DYNAMIC,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pRelaDyn      = pLinker.getOrCreateSection(".rela.dyn",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_RELA,
                                               ELF::SHF_ALLOC);
  f_pRelaPlt      = pLinker.getOrCreateSection(".rela.plt",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_RELA,
                                               ELF::SHF_ALLOC);
  f_pRelaDyn      = pLinker.getOrCreateSection(".rel.dyn",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_REL,
                                               ELF::SHF_ALLOC);
  f_pRelaPlt      = pLinker.getOrCreateSection(".rel.plt",
                                               LDFileFormat::ReadOnly,
                                               ELF::SHT_REL,
                                               ELF::SHF_ALLOC);
  f_pGOT          = pLinker.getOrCreateSection(".got",
                                               LDFileFormat::GOT,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPLT          = pLinker.getOrCreateSection(".plt",
                                               LDFileFormat::PLT,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pGOTPLT       = pLinker.getOrCreateSection(".got.plt",
                                               LDFileFormat::GOT,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pPreInitArray = pLinker.getOrCreateSection(".preinit_array",
                                               LDFileFormat::Data,
                                               ELF::SHT_PREINIT_ARRAY,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pInitArray    = pLinker.getOrCreateSection(".init_array",
                                               LDFileFormat::Data,
                                               ELF::SHT_INIT_ARRAY,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pFiniArray    = pLinker.getOrCreateSection(".fini_array",
                                               LDFileFormat::Data,
                                               ELF::SHT_FINI_ARRAY,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pCtors        = pLinker.getOrCreateSection(".ctors",
                                               LDFileFormat::Data,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pDtors        = pLinker.getOrCreateSection(".dtors",
                                               LDFileFormat::Data,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);
}

