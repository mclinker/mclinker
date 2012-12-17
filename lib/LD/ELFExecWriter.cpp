//===- ELFExecWriter.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFExecWriter.h>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryArea.h>

#include <llvm/Support/ELF.h>
#include <vector>

using namespace llvm;
using namespace mcld;


//===----------------------------------------------------------------------===//
// ELFExecWriter
//===----------------------------------------------------------------------===//
ELFExecWriter::ELFExecWriter(GNULDBackend& pBackend,
                             const LinkerConfig& pConfig)
  : ExecWriter(pBackend), ELFWriter(pBackend),
    m_Config(pConfig) {

}

ELFExecWriter::~ELFExecWriter()
{
}

llvm::error_code ELFExecWriter::writeExecutable(Module& pModule,
                                                MemoryArea& pOutput)
{
  // write out the interpreter section: .interp
  target().emitInterp(pOutput);

  // Write out name pool sections: .dynsym, .dynstr, .hash
  target().emitDynNamePools(pModule, pOutput);

  // Write out name pool sections: .symtab, .strtab
  target().emitRegNamePools(pModule, pOutput);

  // Write out regular ELF sections
  Module::iterator sect, sectEnd = pModule.end();
  for (sect = pModule.begin(); sect != sectEnd; ++sect) {
    MemoryRegion* region = NULL;
    // request output region
    switch((*sect)->kind()) {
      case LDFileFormat::Note:
        if ((*sect)->getSectionData() == NULL)
          continue;
        // Fall through
      case LDFileFormat::Regular:
      case LDFileFormat::Relocation:
      case LDFileFormat::Target:
      case LDFileFormat::Debug:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::EhFrame: {
        region = pOutput.request((*sect)->offset(), (*sect)->size());
        if (NULL == region) {
          llvm::report_fatal_error(llvm::Twine("cannot get enough memory region for output section `") +
                                   llvm::Twine((*sect)->name()) +
                                   llvm::Twine("'.\n"));
        }
        break;
      }
      case LDFileFormat::Null:
      case LDFileFormat::NamePool:
      case LDFileFormat::BSS:
      case LDFileFormat::MetaData:
      case LDFileFormat::Version:
      case LDFileFormat::EhFrameHdr:
      case LDFileFormat::StackNote:
        // ignore these sections
        continue;
      default: {
        llvm::errs() << "WARNING: unsupported section kind: "
                     << (*sect)->kind()
                     << " of section "
                     << (*sect)->name()
                     << ".\n";
        continue;
      }
    }

    // write out sections with data
    switch((*sect)->kind()) {
      case LDFileFormat::Regular:
      case LDFileFormat::Debug:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::Note:
      case LDFileFormat::EhFrame: {
        // FIXME: if optimization of exception handling sections is enabled,
        // then we should emit these sections by the other way.
        emitSectionData(**sect, *region);
        break;
      }
      case LDFileFormat::Relocation:
        emitRelocation(m_Config, **sect, *region);
        break;
      case LDFileFormat::Target:
        target().emitSectionData(**sect, *region);
        break;
      default:
        continue;
    }
  } // end of for loop

  emitELFShStrTab(target().getOutputFormat()->getShStrTab(),
                  pModule,
                  pOutput);

  if (m_Config.targets().is32Bits()) {
    // Write out ELF header
    // Write out section header table
    writeELF32Header(m_Config, pModule, pOutput);

    emitELF32ProgramHeader(pOutput);

    emitELF32SectionHeader(pModule, m_Config, pOutput);
  }
  else if (m_Config.targets().is64Bits()) {
    // Write out ELF header
    // Write out section header table
    writeELF64Header(m_Config, pModule, pOutput);

    emitELF64ProgramHeader(pOutput);

    emitELF64SectionHeader(pModule, m_Config, pOutput);
  }
  else
    return make_error_code(errc::not_supported);

  pOutput.clear();
  return llvm::make_error_code(llvm::errc::success);
}

