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
  // Write out ELF header
  if (32 == target().bitclass())
    writeELF32Header(m_Linker.getLDInfo(), m_Linker.getLayout(), target(), pOutput);
  else if (64 == target().bitclass())
    writeELF64Header(m_Linker.getLDInfo(), m_Linker.getLayout(), target(), pOutput);
  else
    return make_error_code(errc::not_supported);

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
    switch(sect->type()) {
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
      case LDFileFormat::NamePool:
      case LDFileFormat::BSS:
        // ignore these sections
        continue;
      default: {
        llvm::errs() << "WARNING: unsupported section type: "
                     << sect->type()
                     << ".\n";
        continue;
      }
    }

    // write out sections with data
    switch(sect->type()) {
      case LDFileFormat::Regular: {
        cur_offset += emitSectionData(*sect, *region);
        break;
      }
      case LDFileFormat::Relocation:
        cur_offset += emitRelocation(*sect, *region);
        break;
      case LDFileFormat::Target:
        cur_offset += target().emitSectionData(*sect, *region);
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

  // Write out section header table
  emitSectionHeader(pOutput, m_Linker, cur_offset);

  return llvm::make_error_code(llvm::errc::success);
}

