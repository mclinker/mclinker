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
  SectionFactory& alloc = pLinker.getSectFactory();

  f_pELFNULLSection  = alloc.produce("",
                                     LDFileFormat::MetaData,
                                     ELF::SHT_NULL,
                                     0);
  f_pTextSection     = alloc.produce(".text",
                                     LDFileFormat::Text,
                                     ELF::SHT_PROGBITS,
                                     ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  f_pDataSection     = alloc.produce(".data",
                                     LDFileFormat::Data,
                                     ELF::SHT_PROGBITS,
                                     ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pBSSSection      = alloc.produce(".bss",
                                     LDFileFormat::BSS,
                                     ELF::SHT_NOBITS,
                                     ELF::SHF_ALLOC | ELF::SHF_WRITE);
  f_pReadOnlySection = alloc.produce(".rodata",
                                     LDFileFormat::ReadOnly,
                                     ELF::SHT_PROGBITS,
                                     ELF::SHF_ALLOC);
  // In ELF Spec Book I, p1-16. If symbol table and string table are in 
  // loadable segments, set the attribute to SHF_ALLOC bit. But in the
  // real world, this bit always turn off.
  f_pELFSymTab       = alloc.produce(".symtab",
                                     LDFileFormat::SymbolTable,
                                     ELF::SHT_SYMTAB,
                                     0);
  f_pELFStrTab       = alloc.produce(".strtab",
                                     LDFileFormat::StringTable,
                                     ELF::SHT_STRTAB,
                                     0);
  f_pELFComment      = alloc.produce(".comment",
                                     LDFileFormat::MetaData,
                                     ELF::SHT_PROGBITS,
                                     0);
  f_pELFSectStrTab   = alloc.produce(".shstrtab",
                                     LDFileFormat::StringTable,
                                     ELF::SHT_STRTAB,
                                     0);
}

