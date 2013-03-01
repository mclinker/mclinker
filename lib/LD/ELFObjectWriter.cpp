//===- ELFObjectWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFObjectWriter.h>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryArea.h>

#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/system_error.h>

using namespace llvm;
using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFObjectWriter
//===----------------------------------------------------------------------===//
ELFObjectWriter::ELFObjectWriter(GNULDBackend& pBackend,
                                 const LinkerConfig& pConfig)
  : ObjectWriter(), ELFWriter(pBackend),
    m_Config(pConfig) {
}

ELFObjectWriter::~ELFObjectWriter()
{
}

void ELFObjectWriter::writeSection(MemoryArea& pOutput, LDSection *section)
{
  MemoryRegion* region;
  // Request output region
  switch (section->kind()) {
  case LDFileFormat::Note:
    if (section->getSectionData() == NULL)
      return;
    // Fall through
  case LDFileFormat::Regular:
  case LDFileFormat::Relocation:
  case LDFileFormat::Target:
  case LDFileFormat::Debug:
  case LDFileFormat::GCCExceptTable:
  case LDFileFormat::EhFrame: {
    region = pOutput.request(section->offset(), section->size());
    if (NULL == region) {
      llvm::report_fatal_error(llvm::Twine("cannot get enough memory region for output section `") +
                               llvm::Twine(section->name()) +
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
    // Ignore these sections
    return;
  default:
    llvm::errs() << "WARNING: unsupported section kind: "
                 << section->kind()
                 << " of section "
                 << section->name()
                 << ".\n";
    return;
  }

  // Write out sections with data
  switch(section->kind()) {
  case LDFileFormat::GCCExceptTable:
  case LDFileFormat::EhFrame:
  case LDFileFormat::Regular:
  case LDFileFormat::Debug:
  case LDFileFormat::Note:
    // FIXME: if optimization of exception handling sections is enabled,
    // then we should emit these sections by the other way.
    emitSectionData(*section, *region);
    break;
  case LDFileFormat::Relocation:
    emitRelocation(m_Config, *section, *region);
    break;
  case LDFileFormat::Target:
    target().emitSectionData(*section, *region);
    break;
  default:
    llvm_unreachable("invalid section kind");
  }
}

llvm::error_code ELFObjectWriter::writeObject(Module& pModule,
                                              MemoryArea& pOutput)
{
  bool is_dynobj = m_Config.codeGenType() == LinkerConfig::DynObj;
  bool is_exec = m_Config.codeGenType() == LinkerConfig::Exec;
  bool is_binary = m_Config.codeGenType() == LinkerConfig::Binary;
  bool is_object = m_Config.codeGenType() == LinkerConfig::Object;

  assert(is_dynobj || is_exec || is_binary || is_object);

  if (is_dynobj || is_exec) {
    // Write out the interpreter section: .interp
    target().emitInterp(pOutput);

    // Write out name pool sections: .dynsym, .dynstr, .hash
    target().emitDynNamePools(pModule, pOutput);
  }

  if (is_object || is_dynobj || is_exec) {
    // Write out name pool sections: .symtab, .strtab
    target().emitRegNamePools(pModule, pOutput);
  }

  if (is_binary) {
    // Iterate over the loadable segments and write the corresponding sections
    ELFSegmentFactory::iterator seg, segEnd = target().elfSegmentTable().end();

    for (seg = target().elfSegmentTable().begin(); seg != segEnd; ++seg) {
      if (llvm::ELF::PT_LOAD == (*seg).type()) {
        ELFSegment::sect_iterator sect, sectEnd = (*seg).end();
        for (sect = (*seg).begin(); sect != sectEnd; ++sect)
          writeSection(pOutput, *sect);
      }
    }
  } else {
    // Write out regular ELF sections
    Module::iterator sect, sectEnd = pModule.end();
    for (sect = pModule.begin(); sect != sectEnd; ++sect)
      writeSection(pOutput, *sect);

    emitELFShStrTab(target().getOutputFormat()->getShStrTab(),
                    pModule,
                    pOutput);

    if (m_Config.targets().is32Bits()) {
      // Write out ELF header
      // Write out section header table
      writeELF32Header(m_Config,
                       pModule,
                       pOutput);
      if (is_dynobj || is_exec)
        emitELF32ProgramHeader(pOutput);

      emitELF32SectionHeader(pModule, m_Config, pOutput);
    }
    else if (m_Config.targets().is64Bits()) {
      // Write out ELF header
      // Write out section header table
      writeELF64Header(m_Config,
                       pModule,
                       pOutput);
      if (is_dynobj || is_exec)
        emitELF64ProgramHeader(pOutput);

      emitELF64SectionHeader(pModule, m_Config, pOutput);
    }
    else
      return make_error_code(errc::not_supported);
  }

  pOutput.clear();
  return llvm::make_error_code(llvm::errc::success);
}

