//===- ELFDynObjWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFDynObjWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/ScopedWriter.h>
#include <llvm/Support/ELF.h>
#include <vector>

using namespace llvm;
using namespace mcld;

//==========================
// ELFDynObjWriter
ELFDynObjWriter::ELFDynObjWriter(GNULDBackend& pBackend, MCLinker& pLinker)
  : DynObjWriter(pBackend),
    ELFWriter(pBackend),
    m_Backend(pBackend),
    m_Linker(pLinker) {

}

ELFDynObjWriter::~ELFDynObjWriter()
{
}

llvm::error_code ELFDynObjWriter::writeDynObj(Output& pOutput)
{
  // FIXME:
  // Write out ELF program header

  // FIXME:
  // Write out ELF segments

  // Write out ELF sections in the segment
  uint64_t cur_offset = target().sectionStartOffset();

  // Write out regular ELF sections
  unsigned int secIdx = 0;
  unsigned int secEnd = pOutput.context()->numOfSections();
  for (secIdx = 0; secIdx < secEnd; ++secIdx) {
    LDSection* sect = pOutput.context()->getSection(secIdx);
    MemoryRegion* region = NULL;
    // request output region
    switch(sect->kind()) {
      case LDFileFormat::Regular:
      case LDFileFormat::Relocation:
      case LDFileFormat::Target: {
        region = pOutput.memArea()->request(sect->offset(),
                                            sect->size(),
                                            true);
        if (NULL == region) {
          llvm::report_fatal_error(llvm::Twine("cannot get enough memory region for output section[") +
                                   llvm::Twine(secIdx) +
                                   llvm::Twine("] - `") +
                                   sect->name() +
                                   llvm::Twine("'.\n"));
        }
        break;
      }
      case LDFileFormat::Null:
      case LDFileFormat::NamePool:
      case LDFileFormat::BSS:
      case LDFileFormat::Debug:
      case LDFileFormat::Note:
      case LDFileFormat::MetaData:
        // ignore these sections
        continue;
      default: {
        llvm::errs() << "WARNING: unsupported section kind: "
                     << sect->kind()
                     << " of section "
                     << sect->name()
                     << ".\n";
        continue;
      }
    }

    // write out sections with data
    switch(sect->kind()) {
      case LDFileFormat::Regular: {
        cur_offset += emitSectionData(*sect, *region);
        break;
      }
      case LDFileFormat::Relocation:
        cur_offset += emitRelocation(*sect, *region);
        break;
      case LDFileFormat::Target:
        cur_offset += target().emitSectionData(pOutput,
                                               *sect,
                                               m_Linker.getLDInfo(),
                                               *region);
        break;
      default:
        continue;
    }
  }

  // Write out name pool sections: .dynsym, .dynstr, .hash
  cur_offset += target().emitDynNamePools(pOutput, m_Linker.getLDInfo());

  // Write out name pool sections: .symtab, .strtab
  // FIXME: remove this after testing.
  cur_offset += target().emitRegNamePools(pOutput, m_Linker.getLDInfo());

  // FIXME:
  // Write out dynamic section .dynamic

  // Write out .shstrtab
  m_Linker.createSectHdr(".shstrtab",
                         LDFileFormat::NamePool,
                         llvm::ELF::SHT_STRTAB,
                         0x0);
  LDSection* shstrtab = pOutput.context()->getSection(".shstrtab");
  cur_offset += emitShStrTab(pOutput, *shstrtab, cur_offset);

  // Write out ELF header and section header table
  if (32 == target().bitclass()) {
    writeELF32Header(m_Linker.getLDInfo(),
                     m_Linker.getLayout(),
                     target(),
                     pOutput,
                     cur_offset);

    cur_offset += emitELF32SectionHeader(pOutput, m_Linker, cur_offset);
  }
  else if (64 == target().bitclass()) {
    writeELF64Header(m_Linker.getLDInfo(),
                     m_Linker.getLayout(),
                     target(),
                     pOutput,
                     cur_offset);

    cur_offset += emitELF64SectionHeader(pOutput, m_Linker, cur_offset);
  }
  else
    return make_error_code(errc::not_supported);

  return llvm::make_error_code(llvm::errc::success);
}

