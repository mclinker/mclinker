//===- ELFWriter.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ELF.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/ELFWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/Layout.h>
#include <mcld/Target/GNULDBackend.h>
#include <cstdlib>

using namespace llvm;
using namespace mcld;

ELFWriter::FileOffset ELFWriter::writeELF32Header(const MCLDInfo& pLDInfo,
                                                  const Layout& pLayout,
                                                  const GNULDBackend& pBackend,
                                                  Output& pOutput,
                                                  FileOffset pShdrOffset) const
{
    assert(pOutput.hasMemArea());

    ELF::Elf32_Ehdr elf_header;

    memcpy(elf_header.e_ident, ELF::ElfMagic, strlen(ELF::ElfMagic));

    elf_header.e_ident[ELF::EI_CLASS] = ELF::ELFCLASS32;
    elf_header.e_ident[ELF::EI_DATA] = pBackend.isLittleEndian() ?
                                                         ELF::ELFDATA2LSB : ELF::ELFDATA2MSB;
    elf_header.e_ident[ELF::EI_VERSION] = pBackend.ELFVersion();
    elf_header.e_ident[ELF::EI_OSABI] = pBackend.OSABI();
    elf_header.e_ident[ELF::EI_ABIVERSION] = pBackend.ABIVersion();

    // FIXME: add processor-specific and core file types.
    switch(pOutput.type()) {
    case Output::Object:
      elf_header.e_type = ELF::ET_REL;
      break;
    case Output::DynObj:
      elf_header.e_type = ELF::ET_DYN;
      break;
    case Output::Exec:
      elf_header.e_type = ELF::ET_EXEC;
      break;
    default:
      errs() << "unspported output file type: " << pOutput.type() << ".\n";
      elf_header.e_type = ELF::ET_NONE;
    }
    elf_header.e_machine   = pBackend.machine();
    elf_header.e_version   = elf_header.e_ident[ELF::EI_VERSION];
    elf_header.e_entry     = getEntryPoint(pLDInfo, pLayout, pBackend, pOutput);
    elf_header.e_phoff     = sizeof(ELF::Elf32_Ehdr);
    elf_header.e_shoff     = pShdrOffset;
    elf_header.e_flags     = pBackend.flags();
    elf_header.e_ehsize    = sizeof(ELF::Elf32_Ehdr);
    elf_header.e_phentsize = sizeof(ELF::Elf32_Phdr);
    elf_header.e_phnum     = pLayout.numOfSegments();
    elf_header.e_shentsize = sizeof(ELF::Elf32_Shdr);
    elf_header.e_shnum     = pOutput.context()->numOfSections();
    elf_header.e_shstrndx  = pOutput.context()->getSectionIdx(".shstrtab");

    MemoryRegion *mr = pOutput.memArea()->request(0,
                                                  sizeof(ELF::Elf32_Ehdr),
                                                  true);
    memcpy(mr->start(), &elf_header, sizeof(ELF::Elf32_Ehdr));
    mr->sync();
    return sizeof(ELF::Elf32_Ehdr);
}

ELFWriter::FileOffset ELFWriter::writeELF64Header(const MCLDInfo& pLDInfo,
                                                  const Layout& pLayout,
                                                  const GNULDBackend& pBackend,
                                                  Output& pOutput,
                                                  FileOffset pShdrOffset) const
{
    assert(pOutput.hasMemArea());

    ELF::Elf64_Ehdr elf_header;

    memcpy(elf_header.e_ident, ELF::ElfMagic, strlen(ELF::ElfMagic));

    elf_header.e_ident[ELF::EI_CLASS] = ELF::ELFCLASS64;
    elf_header.e_ident[ELF::EI_DATA] = pBackend.isLittleEndian() ?
                                                         ELF::ELFDATA2LSB : ELF::ELFDATA2MSB;
    elf_header.e_ident[ELF::EI_VERSION] = pBackend.ELFVersion();
    elf_header.e_ident[ELF::EI_OSABI] = pBackend.OSABI();
    elf_header.e_ident[ELF::EI_ABIVERSION] = pBackend.ABIVersion();

    // FIXME: add processor-specific and core file types.
    switch(pOutput.type()) {
    case Output::Object:
      elf_header.e_type = ELF::ET_REL;
      break;
    case Output::DynObj:
      elf_header.e_type = ELF::ET_DYN;
      break;
    case Output::Exec:
      elf_header.e_type = ELF::ET_EXEC;
      break;
    default:
      errs() << "unspported output file type: " << pOutput.type() << ".\n";
      elf_header.e_type = ELF::ET_NONE;
    }
    elf_header.e_machine   = pBackend.machine();
    elf_header.e_version   = elf_header.e_ident[ELF::EI_VERSION];
    elf_header.e_entry     = getEntryPoint(pLDInfo, pLayout, pBackend, pOutput);
    elf_header.e_phoff     = sizeof(ELF::Elf64_Ehdr);
    elf_header.e_shoff     = pShdrOffset;
    elf_header.e_flags     = pBackend.flags();
    elf_header.e_ehsize    = sizeof(ELF::Elf64_Ehdr);
    elf_header.e_phentsize = sizeof(ELF::Elf64_Phdr);
    elf_header.e_phnum     = pLayout.numOfSegments();
    elf_header.e_shentsize = sizeof(ELF::Elf64_Shdr);
    elf_header.e_shnum     = pOutput.context()->numOfSections();
    elf_header.e_shstrndx  = pOutput.context()->getSectionIdx(".shstrtab");

    MemoryRegion *mr = pOutput.memArea()->request(0, sizeof(ELF::Elf64_Ehdr), true);
    memcpy(mr->start(), &elf_header, sizeof(ELF::Elf64_Ehdr));
    mr->sync();
    return sizeof(ELF::Elf64_Ehdr);
}

uint64_t ELFWriter::getEntryPoint(const MCLDInfo& pLDInfo,
                                  const Layout& pLayout,
                                  const GNULDBackend& pBackend,
                                  const Output& pOutput) const
{

  llvm::StringRef entry_name;
  if (pLDInfo.options().hasEntry())
    entry_name = pLDInfo.options().entry();
  else
    entry_name = pBackend.entry();
  
  uint64_t result = 0x0;

  bool issue_warning = (pLDInfo.options().hasEntry()
                       && (pOutput.type() != Output::Object)
                       && (pOutput.type() != Output::DynObj));

  const LDSymbol* entry_symbol = pLDInfo.getStrSymPool().findSymbol(entry_name);

  // found the symbol
  if (NULL != entry_symbol) {
    if (entry_symbol->desc() != ResolveInfo::Define && issue_warning) {
      llvm::errs() << "WARNING: entry symbol '"
                   << entry_symbol->name()
                   << "' exists but is not defined.\n";
    }
    result = entry_symbol->value();
  }
  // not in the symbol pool
  else {
    // We should parse entry as a number.
    // @ref GNU ld manual, Options -e. e.g., -e 0x1000.
    char* endptr;
    result = strtoull(entry_name.data(), &endptr, 0);
    if (*endptr != '\0') {
      if (issue_warning) {
        llvm::errs() << "cannot find entry symbol '"
                     << entry_name.data()
                     << "'.\n";
      }
      result = 0x0;
    }
  }
  return result;
}

ELFWriter::FileOffset ELFWriter::emitShStrTab(Output& pOutput,
                                              LDSection& pShStrSect,
                                              FileOffset pStartOffset) const

{
  // compute size
  unsigned int shstrsize = 0;
  LDContext::const_sect_iterator section, sectEnd = pOutput.context()->sectEnd();
  for (section = pOutput.context()->sectBegin(); section != sectEnd; ++section) {
    shstrsize += (*section)->name().size() + 1;
  }

  // write out data
  MemoryRegion* region = pOutput.memArea()->request(pStartOffset, shstrsize, true);
  unsigned char* data = region->start();
  shstrsize = 0;
  for (section = pOutput.context()->sectBegin(); section != sectEnd; ++section) {
    strcpy((char*)(data + shstrsize), (*section)->name().c_str());
    shstrsize += (*section)->name().size() + 1;
  }
  region->sync();

  pShStrSect.setKind(LDFileFormat::NamePool);
  pShStrSect.setType(llvm::ELF::SHT_STRTAB);
  pShStrSect.setFlag(0x0);
  pShStrSect.setSize(shstrsize);
  pShStrSect.setOffset(pStartOffset);
  pShStrSect.setAddr(0x0);
  return shstrsize;
}

ELFWriter::FileOffset ELFWriter::emitELF32SectionHeader(Output& pOutput,
                                                        MCLinker& pLinker,
                                                        FileOffset pStartOffset) const

{
  // emit section header
  unsigned int sectNum = pOutput.context()->numOfSections();
  unsigned int header_size = sizeof(llvm::ELF::Elf32_Shdr) * sectNum;
  MemoryRegion* header_region = pOutput.memArea()->request(pStartOffset,
                                                           header_size,
                                                           true);
  llvm::ELF::Elf32_Shdr* shdr = (llvm::ELF::Elf32_Shdr*)header_region->start();

  // Iterate the SectionTable in LDContext
  unsigned int sectIdx = 0;
  unsigned int shstridx = 0; // NULL section has empty name
  for (; sectIdx < sectNum; ++sectIdx) {
    const LDSection *ld_sect = pOutput.context()->getSection(sectIdx);
    shdr[sectIdx].sh_name      = shstridx;
    shdr[sectIdx].sh_type      = ld_sect->type();
    shdr[sectIdx].sh_flags     = ld_sect->flag();
    shdr[sectIdx].sh_addr      = ld_sect->addr();
    shdr[sectIdx].sh_offset    = ld_sect->offset();
    shdr[sectIdx].sh_size      = ld_sect->size();
    shdr[sectIdx].sh_addralign = (ld_sect->hasSectionData())?
                                   ld_sect->getSectionData()->getAlignment():
                                   0x0;

    shdr[sectIdx].sh_entsize   = getSectEntrySize(*ld_sect);
    shdr[sectIdx].sh_link      = getSectLink(*ld_sect, pOutput);
    shdr[sectIdx].sh_info      = getSectInfo(*ld_sect, pOutput);

    // adjust strshidx
    shstridx += ld_sect->name().size() + 1;
  }
  header_region->sync();
  return header_size;
}

ELFWriter::FileOffset ELFWriter::emitELF64SectionHeader(Output& pOutput,
                                                        MCLinker& pLinker,
                                                        FileOffset pStartOffset) const
{
  // emit section header
  unsigned int sectNum = pOutput.context()->numOfSections();
  unsigned int header_size = sizeof(llvm::ELF::Elf64_Shdr) * sectNum;
  MemoryRegion* header_region = pOutput.memArea()->request(pStartOffset,
                                                           header_size,
                                                           true);
  llvm::ELF::Elf64_Shdr* shdr = (llvm::ELF::Elf64_Shdr*)header_region->start();

  // Iterate the SectionTable in LDContext
  unsigned int sectIdx = 0;
  unsigned int shstridx = 0; // NULL section has empty name
  for (; sectIdx < sectNum; ++sectIdx) {
    const LDSection *ld_sect = pOutput.context()->getSection(sectIdx);
    shdr[sectIdx].sh_name      = shstridx;
    shdr[sectIdx].sh_type      = ld_sect->type();
    shdr[sectIdx].sh_flags     = ld_sect->flag();
    shdr[sectIdx].sh_addr      = ld_sect->addr();
    shdr[sectIdx].sh_offset    = ld_sect->offset();
    shdr[sectIdx].sh_size      = ld_sect->size();
    shdr[sectIdx].sh_addralign = (ld_sect->hasSectionData())?
                                   ld_sect->getSectionData()->getAlignment():
                                   0x0;

    shdr[sectIdx].sh_entsize   = getSectEntrySize(*ld_sect);
    shdr[sectIdx].sh_link      = getSectLink(*ld_sect, pOutput);
    shdr[sectIdx].sh_info      = getSectInfo(*ld_sect, pOutput);

    // adjust strshidx
    shstridx += ld_sect->name().size() + 1;
  }
  header_region->sync();
  return header_size;
}

ELFWriter::FileOffset ELFWriter::emitSectionData(const LDSection& pSection,
                                                 MemoryRegion& pRegion) const
{
  // TODO
  return 0x0;
}
  
ELFWriter::FileOffset ELFWriter::emitRelocation(const LDSection& pSection,
                                                MemoryRegion& pRegion) const
{
  // TODO
  return 0x0;
}
  
ELFWriter::FileOffset ELFWriter::emitRelEntry(const Relocation& pRelocation,
                                              MemoryRegion& pRegion,
                                              FileOffset pOffset,
                                              bool pIsRela) const
{
  // TODO
  return 0x0;
}

// getSectEntrySize - compute ElfXX_Shdr::sh_entsize
uint64_t ELFWriter::getSectEntrySize(const LDSection& pSection) const
{
  // TODO
  return 0x0;
}

// getSectEntrySize - compute ElfXX_Shdr::sh_link
uint64_t ELFWriter::getSectLink(const LDSection& pSection, const Output& pOutput) const
{
  // TODO
  return 0x0;
}

// getSectEntrySize - compute ElfXX_Shdr::sh_info
uint64_t ELFWriter::getSectInfo(const LDSection& pSection, const Output& pOutput) const
{
  // TODO
  return 0x0;
}

