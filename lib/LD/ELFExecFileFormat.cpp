//===- ELFExecFileFormat.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ELF.h>
#include <mcld/LD/ELFExecFileFormat.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSection.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Target/GNULDBackend.h>

using namespace mcld;

void ELFExecFileFormat::initObjectType(MCLinker& pLinker)
{
  // FIXME: make sure ELF executable files has these sections.
  f_pDynSymTab    = &pLinker.getOrCreateOutputSectHdr(".dynsym",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_DYNSYM,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pDynStrTab    = &pLinker.getOrCreateOutputSectHdr(".dynstr",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_STRTAB,
                                           llvm::ELF::SHF_ALLOC,
                                           0x1);
  f_pInterp       = &pLinker.getOrCreateOutputSectHdr(".interp",
                                           LDFileFormat::Note,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC,
                                           0x1);
  f_pHashTab      = &pLinker.getOrCreateOutputSectHdr(".hash",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_HASH,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pDynamic      = &pLinker.getOrCreateOutputSectHdr(".dynamic",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_DYNAMIC,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                           f_Backend.bitclass() / 8);
  f_pRelaDyn      = &pLinker.getOrCreateOutputSectHdr(".rela.dyn",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_RELA,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pRelaPlt      = &pLinker.getOrCreateOutputSectHdr(".rela.plt",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_RELA,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pRelDyn      = &pLinker.getOrCreateOutputSectHdr(".rel.dyn",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_REL,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pRelPlt      = &pLinker.getOrCreateOutputSectHdr(".rel.plt",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_REL,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pGOT          = &pLinker.getOrCreateOutputSectHdr(".got",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                           f_Backend.bitclass() / 8);
  f_pPLT          = &pLinker.getOrCreateOutputSectHdr(".plt",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR,
                                           f_Backend.bitclass() / 8);
  f_pGOTPLT       = &pLinker.getOrCreateOutputSectHdr(".got.plt",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                           f_Backend.bitclass() / 8);
  f_pEhFrameHdr     = &pLinker.getOrCreateOutputSectHdr(".eh_frame_hdr",
                                              LDFileFormat::EhFrameHdr,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC,
                                              0x4);
}
