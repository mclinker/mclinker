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
#include <mcld/LD/ELFWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/Layout.h>
#include <mcld/Target/GNULDBackend.h>

#include <cstdlib>

using namespace llvm;
using namespace mcld;

void ELFWriter::writeELF32Header(const MCLDInfo& pLDInfo,
                                 const Layout& pLayout,
                                 const GNULDBackend& pBackend,
                                 Output& pOutput) const
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
    elf_header.e_entry     = getEntry(pLDInfo, pLayout, pBackend, pOutput);
    elf_header.e_phoff     = sizeof(ELF::Elf32_Ehdr);
    elf_header.e_shoff     = pBackend.sectionStartOffset();
    elf_header.e_flags     = pBackend.flags();
    elf_header.e_ehsize    = sizeof(ELF::Elf32_Ehdr);
    elf_header.e_phentsize = sizeof(ELF::Elf32_Phdr);
    elf_header.e_phnum     = pLayout.numOfSegments();
    elf_header.e_shentsize = sizeof(ELF::Elf32_Shdr);
    elf_header.e_shnum     = pLayout.numOfSections();
    elf_header.e_shstrndx  = pLayout.numOfSections() - 1;

    MemoryRegion *mr = pOutput.memArea()->request(0, sizeof(ELF::Elf32_Ehdr));
    memcpy(mr->start(), &elf_header, sizeof(ELF::Elf32_Ehdr));
    mr->sync();
}

void ELFWriter::writeELF64Header(const MCLDInfo& pLDInfo,
                                 const Layout& pLayout,
                                 const GNULDBackend& pBackend,
                                 Output& pOutput) const
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
    elf_header.e_entry     = getEntry(pLDInfo, pLayout, pBackend, pOutput);
    elf_header.e_phoff     = sizeof(ELF::Elf64_Ehdr);
    elf_header.e_shoff     = pBackend.sectionStartOffset();
    elf_header.e_flags     = pBackend.flags();
    elf_header.e_ehsize    = sizeof(ELF::Elf64_Ehdr);
    elf_header.e_phentsize = sizeof(ELF::Elf64_Phdr);
    elf_header.e_phnum     = pLayout.numOfSegments();
    elf_header.e_shentsize = sizeof(ELF::Elf64_Shdr);
    elf_header.e_shnum     = pLayout.numOfSections();
    elf_header.e_shstrndx  = pLayout.numOfSections() - 1;

    MemoryRegion *mr = pOutput.memArea()->request(0, sizeof(ELF::Elf64_Ehdr));
    memcpy(mr->start(), &elf_header, sizeof(ELF::Elf64_Ehdr));
    mr->sync();
}

uint64_t ELFWriter::getEntry(const MCLDInfo& pLDInfo,
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


ELFWriter::FileOffset ELFWriter::emitSectionHeader(Output& pOutput,
                                                   MCLinker& pLinker,
                                                   FileOffset pStartOffset) const

{
  // TODO
  return 0x0;
/**
  size_t header_size = 0;
  if ( 32 == target().bitclass())
    header_size = sizeof(llvm::Elf32_Shdr) * pOutput.context()->numOfSections();
  else if (64 == target().bitclass())
    header_size = sizeof(llvm::Elf64_Shdr) * pOutput.context()->numOfSections();
  else
    llvm::report_fatal_error(llvm::Twine("invalid ELF target.") +
                             llvm::Twine("ELF supports only 32 and 64 bits target machine.\n"));
  
  MemoryRegion* header_region = pOutput->memArea()->request(cur_offset, header_size);
  // TODO
  uint32_t sec_off = 0;

  // Iterate the SectionTable in LDContext
  LDContext::const_sect_iterator sect_iter;
  for (sect_iter = m_pContext->sectBegin(); sect_iter < m_pContext->sectEnd(); ++sect_iter) {
      LDSection *section = *sect_iter;
      sec_off += WriteSectionEnrty(section, file_offset + sec_off);


   SectionExtInfo & sec_ext_info = getOrCreateSectionExtInfo(section);

    ELF::Elf32_Shdr sec_header;
    sec_header.sh_name      = sec_ext_info.sh_name;
    sec_header.sh_type      = section->type();
    sec_header.sh_flags     = section->flag();
    sec_header.sh_addr      = section->addr();
    sec_header.sh_offset    = section->offset();
    sec_header.sh_size      = section->size();
    sec_header.sh_link      = sec_ext_info.sh_link;
    sec_header.sh_info      = sec_ext_info.sh_info;

    if (NULL != section->getSectionData())
        sec_header.sh_addralign = section->getSectionData()->getAlignment();
    else
        sec_header.sh_addralign = 0x0;

    sec_header.sh_entsize   = sec_ext_info.sh_entsize;

    MemoryRegion *mr = m_pMemArea->request(file_offset, sizeof(ELF::Elf32_Shdr));
    memcpy(mr->start(), &sec_header, sizeof(ELF::Elf32_Shdr));
    mr->sync();

    return  sizeof(ELF::Elf32_Shdr);

  }
  **/
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

