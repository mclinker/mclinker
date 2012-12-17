//===- ELFBinaryWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFBinaryWriter.h>

#include <mcld/Module.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/LD/ELFSegmentFactory.h>
#include <mcld/LD/ELFSegment.h>

#include <llvm/Support/system_error.h>
using namespace llvm;
using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFBinaryWriter
//===----------------------------------------------------------------------===//
ELFBinaryWriter::ELFBinaryWriter(GNULDBackend& pBackend,
                                 const LinkerConfig& pConfig)
  : BinaryWriter(pBackend), ELFWriter(pBackend), m_Config(pConfig) {
}

ELFBinaryWriter::~ELFBinaryWriter()
{
}

llvm::error_code ELFBinaryWriter::writeBinary(Module& pModule,
                                              MemoryArea& pOutput)
{
  // Write out regular ELF sections
  for (ELFSegmentFactory::iterator seg = target().elfSegmentTable().begin(),
         segEnd = target().elfSegmentTable().end(); seg != segEnd; ++seg) {
    if (llvm::ELF::PT_LOAD != (*seg).type())
      continue;

    for (ELFSegment::sect_iterator sect = (*seg).begin(),
           sectEnd = (*seg).end(); sect != sectEnd; ++sect) {
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
    } // end of section for loop
  } // end of segment for loop
  return llvm::make_error_code(llvm::errc::success);
}

