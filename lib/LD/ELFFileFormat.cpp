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

ELFFileFormat::ELFFileFormat()
  : f_pNULLSection(0),
    f_pSymTab(0),
    f_pStrTab(0),
    f_pComment(0) {

}

ELFFileFormat::~ELFFileFormat()
{
}

void ELFFileFormat::initObjectFormat(MCLinker& pLinker)
{
  f_pTextSection     = &pLinker.getOrCreateOutputSectHdr(".text",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR);
  f_pNULLSection     = &pLinker.getOrCreateOutputSectHdr("",
                                              LDFileFormat::Null,
                                              llvm::ELF::SHT_NULL,
                                              0);
  f_pReadOnlySection = &pLinker.getOrCreateOutputSectHdr(".rodata",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC);

  f_pBSSSection      = &pLinker.getOrCreateOutputSectHdr(".bss",
                                              LDFileFormat::BSS,
                                              llvm::ELF::SHT_NOBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pComment         = &pLinker.getOrCreateOutputSectHdr(".comment",
                                              LDFileFormat::MetaData,
                                              llvm::ELF::SHT_PROGBITS,
                                              0);
  f_pDataSection     = &pLinker.getOrCreateOutputSectHdr(".data",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pData1           = &pLinker.getOrCreateOutputSectHdr(".data1",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pDebug           = &pLinker.getOrCreateOutputSectHdr(".debug",
                                              LDFileFormat::Debug,
                                              llvm::ELF::SHT_PROGBITS,
                                              0);
  f_pInit            = &pLinker.getOrCreateOutputSectHdr(".init",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR);
  f_pInitArray       = &pLinker.getOrCreateOutputSectHdr(".init_array",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_INIT_ARRAY,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pFini            = &pLinker.getOrCreateOutputSectHdr(".fini",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR);
  f_pFiniArray       = &pLinker.getOrCreateOutputSectHdr(".fini_array",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_FINI_ARRAY,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  f_pLine            = &pLinker.getOrCreateOutputSectHdr(".line",
                                              LDFileFormat::Debug,
                                              llvm::ELF::SHT_PROGBITS,
                                              0);
  f_pPreInitArray    = &pLinker.getOrCreateOutputSectHdr(".preinit_array",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PREINIT_ARRAY,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);
  // the definition of SHF_XXX attributes of rodata in Linux Standard Base
  // conflicts with System V standard. We follow System V standard.
  f_pROData1         = &pLinker.getOrCreateOutputSectHdr(".rodata1",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC);
  f_pShStrTab        = &pLinker.getOrCreateOutputSectHdr(".shstrtab",
                                              LDFileFormat::NamePool,
                                              llvm::ELF::SHT_STRTAB,
                                              0);
  // In ELF Spec Book I, p1-16. If symbol table and string table are in 
  // loadable segments, set the attribute to SHF_ALLOC bit. But in the
  // real world, this bit always turn off.
  f_pSymTab       = &pLinker.getOrCreateOutputSectHdr(".symtab",
                                              LDFileFormat::NamePool,
                                              llvm::ELF::SHT_SYMTAB,
                                              0);
  f_pStrTab       = &pLinker.getOrCreateOutputSectHdr(".strtab",
                                              LDFileFormat::NamePool,
                                              llvm::ELF::SHT_STRTAB,
                                              0);
  f_pTBSS         = &pLinker.getOrCreateOutputSectHdr(".tbss",
                                              LDFileFormat::BSS,
                                              llvm::ELF::SHT_NOBITS,
                                              llvm::ELF::SHF_ALLOC |
                                              llvm::ELF::SHF_WRITE |
                                              llvm::ELF::SHF_TLS);
  f_pTData        = &pLinker.getOrCreateOutputSectHdr(".tdata",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC |
                                              llvm::ELF::SHF_WRITE |
                                              llvm::ELF::SHF_TLS);
}

