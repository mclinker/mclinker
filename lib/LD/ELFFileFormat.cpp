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
#include <mcld/Target/GNULDBackend.h>

using namespace mcld;

ELFFileFormat::ELFFileFormat(GNULDBackend& pBackend)
  : f_Backend(pBackend),
    f_pNULLSection(NULL),
    f_pGOT(NULL),
    f_pPLT(NULL),
    f_pRelDyn(NULL),
    f_pRelPlt(NULL),
    f_pRelaDyn(NULL),
    f_pRelaPlt(NULL),
    f_pComment(NULL),
    f_pData1(NULL),
    f_pDebug(NULL),
    f_pDynamic(NULL),
    f_pDynStrTab(NULL),
    f_pDynSymTab(NULL),
    f_pFini(NULL),
    f_pFiniArray(NULL),
    f_pHashTab(NULL),
    f_pInit(NULL),
    f_pInitArray(NULL),
    f_pInterp(NULL),
    f_pLine(NULL),
    f_pNote(NULL),
    f_pPreInitArray(NULL),
    f_pROData1(NULL),
    f_pShStrTab(NULL),
    f_pStrTab(NULL),
    f_pSymTab(NULL),
    f_pTBSS(NULL),
    f_pTData(NULL),
    f_pCtors(NULL),
    f_pDataRelRo(NULL),
    f_pDtors(NULL),
    f_pEhFrame(NULL),
    f_pEhFrameHdr(NULL),
    f_pGCCExceptTable(NULL),
    f_pGNUVersion(NULL),
    f_pGNUVersionD(NULL),
    f_pGNUVersionR(NULL),
    f_pGOTPLT(NULL),
    f_pJCR(NULL),
    f_pNoteABITag(NULL),
    f_pStab(NULL),
    f_pStabStr(NULL),
    f_pStack(NULL) {

}

ELFFileFormat::~ELFFileFormat()
{
}

void ELFFileFormat::initObjectFormat(MCLinker& pLinker)
{
  f_pTextSection     = &pLinker.getOrCreateOutputSectHdr(".text",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR,
                                              0x1);
  f_pNULLSection     = &pLinker.getOrCreateOutputSectHdr("",
                                              LDFileFormat::Null,
                                              llvm::ELF::SHT_NULL,
                                              0x0);
  f_pReadOnlySection = &pLinker.getOrCreateOutputSectHdr(".rodata",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC,
                                              0x1);

  f_pBSSSection      = &pLinker.getOrCreateOutputSectHdr(".bss",
                                              LDFileFormat::BSS,
                                              llvm::ELF::SHT_NOBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pComment         = &pLinker.getOrCreateOutputSectHdr(".comment",
                                              LDFileFormat::MetaData,
                                              llvm::ELF::SHT_PROGBITS,
                                              0x0,
                                              0x1);
  f_pDataSection     = &pLinker.getOrCreateOutputSectHdr(".data",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pData1           = &pLinker.getOrCreateOutputSectHdr(".data1",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pDebug           = &pLinker.getOrCreateOutputSectHdr(".debug",
                                              LDFileFormat::Debug,
                                              llvm::ELF::SHT_PROGBITS,
                                              0x0,
                                              0x1);
  f_pInit            = &pLinker.getOrCreateOutputSectHdr(".init",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR,
                                              0x1);
  f_pInitArray       = &pLinker.getOrCreateOutputSectHdr(".init_array",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_INIT_ARRAY,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pFini            = &pLinker.getOrCreateOutputSectHdr(".fini",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR,
                                              0x1);
  f_pFiniArray       = &pLinker.getOrCreateOutputSectHdr(".fini_array",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_FINI_ARRAY,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pLine            = &pLinker.getOrCreateOutputSectHdr(".line",
                                              LDFileFormat::Debug,
                                              llvm::ELF::SHT_PROGBITS,
                                              0x0,
                                              0x1);
  f_pPreInitArray    = &pLinker.getOrCreateOutputSectHdr(".preinit_array",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PREINIT_ARRAY,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  // the definition of SHF_XXX attributes of rodata in Linux Standard Base
  // conflicts with System V standard. We follow System V standard.
  f_pROData1         = &pLinker.getOrCreateOutputSectHdr(".rodata1",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC,
                                              0x1);
  f_pShStrTab        = &pLinker.getOrCreateOutputSectHdr(".shstrtab",
                                              LDFileFormat::NamePool,
                                              llvm::ELF::SHT_STRTAB,
                                              0x0,
                                              0x1);
  // In ELF Spec Book I, p1-16. If symbol table and string table are in 
  // loadable segments, set the attribute to SHF_ALLOC bit. But in the
  // real world, this bit always turn off.
  f_pSymTab       = &pLinker.getOrCreateOutputSectHdr(".symtab",
                                              LDFileFormat::NamePool,
                                              llvm::ELF::SHT_SYMTAB,
                                              0x0,
                                              f_Backend.bitclass() / 8);
  f_pStrTab       = &pLinker.getOrCreateOutputSectHdr(".strtab",
                                              LDFileFormat::NamePool,
                                              llvm::ELF::SHT_STRTAB,
                                              0x0,
                                              0x1);
  f_pTBSS         = &pLinker.getOrCreateOutputSectHdr(".tbss",
                                              LDFileFormat::BSS,
                                              llvm::ELF::SHT_NOBITS,
                                              llvm::ELF::SHF_ALLOC |
                                              llvm::ELF::SHF_WRITE |
                                              llvm::ELF::SHF_TLS,
                                              0x1);
  f_pTData        = &pLinker.getOrCreateOutputSectHdr(".tdata",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC |
                                              llvm::ELF::SHF_WRITE |
                                              llvm::ELF::SHF_TLS,
                                              0x1);

  /// @ref 10.3.1.2, ISO/IEC 23360, Part 1:2010(E), p. 24.
  f_pCtors          = &pLinker.getOrCreateOutputSectHdr(".ctor",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pDataRelRo      = &pLinker.getOrCreateOutputSectHdr(".data.rel.ro",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pDtors          = &pLinker.getOrCreateOutputSectHdr(".dtors",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pEhFrame        = &pLinker.getOrCreateOutputSectHdr(".eh_frame",
                                              LDFileFormat::EhFrame,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC,
                                              0x4);
  f_pGCCExceptTable = &pLinker.getOrCreateOutputSectHdr(".gcc_except_table",
                                              LDFileFormat::GCCExceptTable,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC,
                                              0x4);
  f_pGNUVersion     = &pLinker.getOrCreateOutputSectHdr(".gnu.version",
                                              LDFileFormat::Version,
                                              llvm::ELF::SHT_GNU_versym,
                                              llvm::ELF::SHF_ALLOC,
                                              0x1);
  f_pGNUVersionD    = &pLinker.getOrCreateOutputSectHdr(".gnu.version_d",
                                              LDFileFormat::Version,
                                              llvm::ELF::SHT_GNU_verdef,
                                              llvm::ELF::SHF_ALLOC,
                                              0x1);
  f_pGNUVersionR    = &pLinker.getOrCreateOutputSectHdr(".gnu.version_r",
                                              LDFileFormat::Version,
                                              llvm::ELF::SHT_GNU_verneed,
                                              llvm::ELF::SHF_ALLOC,
                                              0x1);
  f_pJCR            = &pLinker.getOrCreateOutputSectHdr(".jcr",
                                              LDFileFormat::Regular,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                              0x1);
  f_pStab           = &pLinker.getOrCreateOutputSectHdr(".stab",
                                              LDFileFormat::Debug,
                                              llvm::ELF::SHT_PROGBITS,
                                              0x0,
                                              0x1);
  f_pStabStr        = &pLinker.getOrCreateOutputSectHdr(".stabstr",
                                              LDFileFormat::Debug,
                                              llvm::ELF::SHT_STRTAB,
                                              0x0,
                                              0x1);
}

