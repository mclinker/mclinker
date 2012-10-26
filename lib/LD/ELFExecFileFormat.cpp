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
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Target/GNULDBackend.h>

using namespace mcld;

void ELFExecFileFormat::initObjectType(FragmentLinker& pLinker)
{
  // FIXME: make sure ELF executable files has these sections.
  f_pDynSymTab    = &pLinker.CreateOutputSectHdr(".dynsym",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_DYNSYM,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pDynStrTab    = &pLinker.CreateOutputSectHdr(".dynstr",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_STRTAB,
                                           llvm::ELF::SHF_ALLOC,
                                           0x1);
  f_pInterp       = &pLinker.CreateOutputSectHdr(".interp",
                                           LDFileFormat::Note,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC,
                                           0x1);
  f_pHashTab      = &pLinker.CreateOutputSectHdr(".hash",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_HASH,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pDynamic      = &pLinker.CreateOutputSectHdr(".dynamic",
                                           LDFileFormat::NamePool,
                                           llvm::ELF::SHT_DYNAMIC,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                           f_Backend.bitclass() / 8);
  f_pRelaDyn      = &pLinker.CreateOutputSectHdr(".rela.dyn",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_RELA,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pRelaPlt      = &pLinker.CreateOutputSectHdr(".rela.plt",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_RELA,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pRelDyn      = &pLinker.CreateOutputSectHdr(".rel.dyn",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_REL,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pRelPlt      = &pLinker.CreateOutputSectHdr(".rel.plt",
                                           LDFileFormat::Relocation,
                                           llvm::ELF::SHT_REL,
                                           llvm::ELF::SHF_ALLOC,
                                           f_Backend.bitclass() / 8);
  f_pGOT          = &pLinker.CreateOutputSectHdr(".got",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                           f_Backend.bitclass() / 8);
  f_pPLT          = &pLinker.CreateOutputSectHdr(".plt",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR,
                                           f_Backend.bitclass() / 8);
  f_pGOTPLT       = &pLinker.CreateOutputSectHdr(".got.plt",
                                           LDFileFormat::Target,
                                           llvm::ELF::SHT_PROGBITS,
                                           llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE,
                                           f_Backend.bitclass() / 8);
  f_pEhFrameHdr     = &pLinker.CreateOutputSectHdr(".eh_frame_hdr",
                                              LDFileFormat::EhFrameHdr,
                                              llvm::ELF::SHT_PROGBITS,
                                              llvm::ELF::SHF_ALLOC,
                                              0x4);
}
