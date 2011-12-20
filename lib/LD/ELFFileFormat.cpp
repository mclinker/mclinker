//===- ELFFileFormat.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFFileFormat.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCLDFile.h>
#include <llvm/Support/ELF.h>

using namespace mcld;
using namespace llvm;

ELFFileFormat::ELFFileFormat()
  : f_pELFNULLSection(0),
    f_pELFSymTab(0),
    f_pELFStrTab(0),
    f_pELFComment(0),
    f_pELFSectStrTab(0) {
}

ELFFileFormat::~ELFFileFormat()
{
}

void ELFFileFormat::initObjectFormat(MCLinker& pLinker)
{
  f_pELFNULLSection  = pLinker.getOrCreateSectHdr("",
                                                  LDFileFormat::MetaData,
                                                  ELF::SHT_NULL,
                                                  0);
  f_pTextSection     = pLinker.getOrCreateSectHdr(".text",
                                                  LDFileFormat::Text,
                                                  ELF::SHT_PROGBITS,
                                                  ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pDataSection     = pLinker.getOrCreateSectHdr(".data",
                                                  LDFileFormat::Data,
                                                  ELF::SHT_PROGBITS,
                                                  ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pBSSSection      = pLinker.getOrCreateSectHdr(".bss",
                                                  LDFileFormat::BSS,
                                                  ELF::SHT_NOBITS,
                                                  ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pReadOnlySection = pLinker.getOrCreateSectHdr(".rodata",
                                                  LDFileFormat::ReadOnly,
                                                  ELF::SHT_PROGBITS,
                                                  ELF::SHF_ALLOC);
  // In ELF Spec Book I, p1-16. If symbol table and string table are in 
  // loadable segments, set the attribute to SHF_ALLOC bit. But in the
  // real world, this bit always turn off.
  f_pELFSymTab       = pLinker.getOrCreateSectHdr(".symtab",
                                                  LDFileFormat::SymbolTable,
                                                  ELF::SHT_SYMTAB,
                                                  0);
  f_pELFStrTab       = pLinker.getOrCreateSectHdr(".strtab",
                                                  LDFileFormat::StringTable,
                                                  ELF::SHT_STRTAB,
                                                  0);
  f_pELFComment      = pLinker.getOrCreateSectHdr(".comment",
                                                  LDFileFormat::MetaData,
                                                  ELF::SHT_PROGBITS,
                                                  0);
  f_pELFSectStrTab   = pLinker.getOrCreateSectHdr(".shstrtab",
                                                  LDFileFormat::StringTable,
                                                  ELF::SHT_STRTAB,
                                                  0);
}

