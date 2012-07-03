//===- ELFExecWriter.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFExecWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <llvm/Support/ELF.h>
#include <vector>

using namespace llvm;
using namespace mcld;


//==========================
// ELFExecWriter
ELFExecWriter::ELFExecWriter(GNULDBackend& pBackend, MCLinker& pLinker)
  : ExecWriter(pBackend),
    ELFWriter(pBackend),
    m_Backend(pBackend),
    m_Linker(pLinker) {

}

ELFExecWriter::~ELFExecWriter()
{
}

llvm::error_code ELFExecWriter::writeExecutable(Output& pOutput)
{
  // write out the interpreter section: .interp
  target().emitInterp(pOutput, m_Linker.getLDInfo());

  // Write out name pool sections: .dynsym, .dynstr, .hash
  target().emitDynNamePools(pOutput,
                            m_Linker.getOutputSymbols(),
                            m_Linker.getLayout(),
                            m_Linker.getLDInfo());

  // Write out name pool sections: .symtab, .strtab
  target().emitRegNamePools(pOutput,
                            m_Linker.getOutputSymbols(),
                            m_Linker.getLayout(),
                            m_Linker.getLDInfo());

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
      case LDFileFormat::Target:
      case LDFileFormat::Debug:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::EhFrame: {
        region = pOutput.memArea()->request(sect->offset(), sect->size());
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
      case LDFileFormat::Note:
      case LDFileFormat::MetaData:
      case LDFileFormat::Version:
      case LDFileFormat::EhFrameHdr:
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
      case LDFileFormat::Regular:
      case LDFileFormat::Debug:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::EhFrame: {
        // FIXME: if optimization of exception handling sections is enabled,
        // then we should emit these sections by the other way.
        emitSectionData(m_Linker.getLayout(), *sect, *region);
        break;
      }
      case LDFileFormat::Relocation:
        emitRelocation(m_Linker.getLayout(), pOutput, *sect, *region);
        break;
      case LDFileFormat::Target:
        target().emitSectionData(pOutput,
                                 *sect,
                                 m_Linker.getLDInfo(),
                                 m_Linker.getLayout(),
                                 *region);
        break;
      default:
        continue;
    }
  } // end of for loop

  if (32 == target().bitclass()) {
    // Write out ELF header
    // Write out section header table
    emitELF32ShStrTab(pOutput, m_Linker);

    writeELF32Header(m_Linker.getLDInfo(),
                     m_Linker.getLayout(),
                     target(),
                     pOutput);

    emitELF32ProgramHeader(pOutput, target());

    emitELF32SectionHeader(pOutput, m_Linker);
  }
  else if (64 == target().bitclass()) {
    // Write out ELF header
    // Write out section header table
    emitELF64ShStrTab(pOutput, m_Linker);

    writeELF64Header(m_Linker.getLDInfo(),
                     m_Linker.getLayout(),
                     target(),
                     pOutput);

    emitELF64ProgramHeader(pOutput, target());

    emitELF64SectionHeader(pOutput, m_Linker);
  }
  else
    return make_error_code(errc::not_supported);

  pOutput.memArea()->clear();
  return llvm::make_error_code(llvm::errc::success);
}

