//===- ELFWriter.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cstdlib>
#include <cstring>

#include <llvm/Support/ELF.h>
#include <llvm/Support/Casting.h>

#include <mcld/ADT/SizeTraits.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/AlignFragment.h>
#include <mcld/LD/FillFragment.h>
#include <mcld/LD/RegionFragment.h>
#include <mcld/LD/ELFWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/Layout.h>
#include <mcld/LD/ELFSegment.h>
#include <mcld/LD/ELFSegmentFactory.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryRegion.h>

using namespace llvm::ELF;
using namespace mcld;

/// writeELF32Header - write ELF header
void ELFWriter::writeELF32Header(const MCLDInfo& pLDInfo,
                                 const Layout& pLayout,
                                 const GNULDBackend& pBackend,
                                 Output& pOutput) const
{
    assert(pOutput.hasMemArea());

    // ELF header must start from 0x0
    MemoryRegion *region = pOutput.memArea()->request(0,
                                                      sizeof(Elf32_Ehdr));
    Elf32_Ehdr* header = (Elf32_Ehdr*)region->start();

    memcpy(header->e_ident, ElfMagic, EI_MAG3+1);

    header->e_ident[EI_CLASS]      = ELFCLASS32;
    header->e_ident[EI_DATA]       = pBackend.isLittleEndian()?
                                       ELFDATA2LSB : ELFDATA2MSB;
    header->e_ident[EI_VERSION]    = pBackend.ELFVersion();
    header->e_ident[EI_OSABI]      = pBackend.OSABI();
    header->e_ident[EI_ABIVERSION] = pBackend.ABIVersion();

    // FIXME: add processor-specific and core file types.
    switch(pOutput.type()) {
    case Output::Object:
      header->e_type = ET_REL;
      break;
    case Output::DynObj:
      header->e_type = ET_DYN;
      break;
    case Output::Exec:
      header->e_type = ET_EXEC;
      break;
    default:
      llvm::errs() << "unspported output file type: " << pOutput.type() << ".\n";
      header->e_type = ET_NONE;
    }
    header->e_machine   = pBackend.machine();
    header->e_version   = header->e_ident[EI_VERSION];
    header->e_entry     = getEntryPoint(pLDInfo, pLayout, pBackend, pOutput);
    header->e_phoff     = sizeof(Elf32_Ehdr);
    header->e_shoff     = getELF32LastStartOffset(pOutput);
    header->e_flags     = pBackend.flags();
    header->e_ehsize    = sizeof(Elf32_Ehdr);
    header->e_phentsize = sizeof(Elf32_Phdr);
    header->e_phnum     = pBackend.numOfSegments();
    header->e_shentsize = sizeof(Elf32_Shdr);
    header->e_shnum     = pOutput.context()->numOfSections();
    header->e_shstrndx  = pOutput.context()->getSectionIdx(".shstrtab");
}

/// writeELF64Header - write ELF header
void ELFWriter::writeELF64Header(const MCLDInfo& pLDInfo,
                                 const Layout& pLayout,
                                 const GNULDBackend& pBackend,
                                 Output& pOutput) const
{
    assert(pOutput.hasMemArea());

    // ELF header must start from 0x0
    MemoryRegion *region = pOutput.memArea()->request(0,
                                                      sizeof(Elf64_Ehdr));
    Elf64_Ehdr* header = (Elf64_Ehdr*)region->start();

    memcpy(header->e_ident, ElfMagic, EI_MAG3+1);

    header->e_ident[EI_CLASS]      = ELFCLASS64;
    header->e_ident[EI_DATA]       = pBackend.isLittleEndian()?
                                       ELFDATA2LSB : ELFDATA2MSB;
    header->e_ident[EI_VERSION]    = pBackend.ELFVersion();
    header->e_ident[EI_OSABI]      = pBackend.OSABI();
    header->e_ident[EI_ABIVERSION] = pBackend.ABIVersion();

    // FIXME: add processor-specific and core file types.
    switch(pOutput.type()) {
    case Output::Object:
      header->e_type = ET_REL;
      break;
    case Output::DynObj:
      header->e_type = ET_DYN;
      break;
    case Output::Exec:
      header->e_type = ET_EXEC;
      break;
    default:
      llvm::errs() << "unspported output file type: " << pOutput.type() << ".\n";
      header->e_type = ET_NONE;
    }
    header->e_machine   = pBackend.machine();
    header->e_version   = header->e_ident[EI_VERSION];
    header->e_entry     = getEntryPoint(pLDInfo, pLayout, pBackend, pOutput);
    header->e_phoff     = sizeof(Elf64_Ehdr);
    header->e_shoff     = getELF64LastStartOffset(pOutput);
    header->e_flags     = pBackend.flags();
    header->e_ehsize    = sizeof(Elf64_Ehdr);
    header->e_phentsize = sizeof(Elf64_Phdr);
    header->e_phnum     = pBackend.numOfSegments();
    header->e_shentsize = sizeof(Elf64_Shdr);
    header->e_shnum     = pOutput.context()->numOfSections();
    header->e_shstrndx  = pOutput.context()->getSectionIdx(".shstrtab");
}

/// getEntryPoint
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

  const LDSymbol* entry_symbol = pLDInfo.getNamePool().findSymbol(entry_name);

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

/// emitELF32SectionHeader - emit Elf32_Shdr
void
ELFWriter::emitELF32SectionHeader(Output& pOutput, MCLinker& pLinker) const
{
  // emit section header
  unsigned int sectNum = pOutput.context()->numOfSections();
  unsigned int header_size = sizeof(Elf32_Shdr) * sectNum;
  MemoryRegion* region = pOutput.memArea()->request(
                                   getELF32LastStartOffset(pOutput),
                                   header_size);
  Elf32_Shdr* shdr = (Elf32_Shdr*)region->start();

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
    shdr[sectIdx].sh_addralign = ld_sect->align();
    shdr[sectIdx].sh_entsize   = getELF32SectEntrySize(*ld_sect);
    shdr[sectIdx].sh_link      = getSectLink(*ld_sect, pOutput);
    shdr[sectIdx].sh_info      = getSectInfo(*ld_sect, pOutput);

    // adjust strshidx
    shstridx += ld_sect->name().size() + 1;
  }
}

/// emitELF64SectionHeader - emit Elf64_Shdr
void
ELFWriter::emitELF64SectionHeader(Output& pOutput, MCLinker& pLinker) const
{
  // emit section header
  unsigned int sectNum = pOutput.context()->numOfSections();
  unsigned int header_size = sizeof(Elf64_Shdr) * sectNum;
  MemoryRegion* region = pOutput.memArea()->request(
                                     getELF64LastStartOffset(pOutput),
                                     header_size);
  Elf64_Shdr* shdr = (Elf64_Shdr*)region->start();

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

    shdr[sectIdx].sh_entsize   = getELF64SectEntrySize(*ld_sect);
    shdr[sectIdx].sh_link      = getSectLink(*ld_sect, pOutput);
    shdr[sectIdx].sh_info      = getSectInfo(*ld_sect, pOutput);

    // adjust strshidx
    shstridx += ld_sect->name().size() + 1;
  }
}


/// emitELF32ProgramHeader - emit Elf32_Phdr
void ELFWriter::emitELF32ProgramHeader(Output& pOutput,
                                       const GNULDBackend& pBackend) const
{
  assert(pOutput.hasMemArea());

  uint64_t start_offset, phdr_size;

  start_offset = sizeof(Elf32_Ehdr);
  phdr_size = sizeof(Elf32_Phdr);
  // Program header must start directly after ELF header
  MemoryRegion *region = pOutput.memArea()->request(start_offset,
                                          pBackend.numOfSegments() * phdr_size);

  Elf32_Phdr* phdr = (Elf32_Phdr*)region->start();

  // Iterate the elf segment table in GNULDBackend
  size_t index = 0;
  ELFSegmentFactory::const_iterator seg = pBackend.elfSegmentTable().begin(),
                                 segEnd = pBackend.elfSegmentTable().end();
  for (; seg != segEnd; ++seg, ++index) {
    phdr[index].p_type   = (*seg).type();
    phdr[index].p_flags  = (*seg).flag();
    phdr[index].p_offset = (*seg).offset();
    phdr[index].p_vaddr  = (*seg).vaddr();
    phdr[index].p_paddr  = (*seg).paddr();
    phdr[index].p_filesz = (*seg).filesz();
    phdr[index].p_memsz  = (*seg).memsz();
    phdr[index].p_align  = (*seg).align();
  }
}

/// emitELF64ProgramHeader - emit ElfR64Phdr
void ELFWriter::emitELF64ProgramHeader(Output& pOutput,
                                       const GNULDBackend& pBackend) const
{
  assert(pOutput.hasMemArea());

  uint64_t start_offset, phdr_size;

  start_offset = sizeof(Elf64_Ehdr);
  phdr_size = sizeof(Elf64_Phdr);
  // Program header must start directly after ELF header
  MemoryRegion *region = pOutput.memArea()->request(start_offset,
                                          pBackend.numOfSegments() * phdr_size);
  Elf64_Phdr* phdr = (Elf64_Phdr*)region->start();

  // Iterate the elf segment table in GNULDBackend
  size_t index = 0;
  ELFSegmentFactory::const_iterator seg = pBackend.elfSegmentTable().begin(),
                                 segEnd = pBackend.elfSegmentTable().end();
  for (; seg != segEnd; ++seg, ++index) {
    phdr[index].p_type   = (*seg).type();
    phdr[index].p_flags  = (*seg).flag();
    phdr[index].p_offset = (*seg).offset();
    phdr[index].p_vaddr  = (*seg).vaddr();
    phdr[index].p_paddr  = (*seg).paddr();
    phdr[index].p_filesz = (*seg).filesz();
    phdr[index].p_memsz  = (*seg).memsz();
    phdr[index].p_align  = (*seg).align();
  }
}

/// emitELF32ShStrTab - emit section string table
void ELFWriter::emitELF32ShStrTab(Output& pOutput, MCLinker& pLinker) const
{
  uint64_t shstroffset = getELF32LastStartOffset(pOutput);

  // get shstrtab
  LDSection& shstrtab = pLinker.getOrCreateOutputSectHdr(".shstrtab",
                                                         LDFileFormat::NamePool,
                                                         SHT_STRTAB,
                                                         0x0);
  if (0 != shstrtab.size())
    llvm::report_fatal_error(".shstrtab has been set.\n");

  // compute size
  unsigned int shstrsize = 0;
  LDContext::const_sect_iterator section, sectEnd = pOutput.context()->sectEnd();
  for (section = pOutput.context()->sectBegin(); section != sectEnd; ++section) {
    shstrsize += (*section)->name().size() + 1;
  }

  shstrtab.setSize(shstrsize);
  shstrtab.setOffset(shstroffset);

  // write out data
  MemoryRegion* region = pOutput.memArea()->request(shstrtab.offset(),
                                                    shstrtab.size());
  unsigned char* data = region->start();
  shstrsize = 0;
  for (section = pOutput.context()->sectBegin(); section != sectEnd; ++section) {
    strcpy((char*)(data + shstrsize), (*section)->name().c_str());
    shstrsize += (*section)->name().size() + 1;
  }

  shstrtab.setKind(LDFileFormat::NamePool);
  shstrtab.setType(llvm::ELF::SHT_STRTAB);
  shstrtab.setFlag(0x0);
  shstrtab.setAddr(0x0);
}


/// emitELF64ShStrTab - emit section string table
void ELFWriter::emitELF64ShStrTab(Output& pOutput, MCLinker& pLinker) const
{
  uint64_t shstroffset = getELF64LastStartOffset(pOutput);

  // get shstrtab
  LDSection& shstrtab = pLinker.getOrCreateOutputSectHdr(".shstrtab",
                                                         LDFileFormat::NamePool,
                                                         SHT_STRTAB,
                                                         0x0);
  if (0 != shstrtab.size())
    llvm::report_fatal_error(".shstrtab has been set.\n");

  // compute offset

  // compute size
  unsigned int shstrsize = 0;
  LDContext::const_sect_iterator section, sectEnd = pOutput.context()->sectEnd();
  for (section = pOutput.context()->sectBegin(); section != sectEnd; ++section) {
    shstrsize += (*section)->name().size() + 1;
  }

  shstrtab.setSize(shstrsize);
  shstrtab.setOffset(shstroffset);

  // write out data
  MemoryRegion* region = pOutput.memArea()->request(shstrtab.offset(),
                                                    shstrtab.size());
  unsigned char* data = region->start();
  shstrsize = 0;
  for (section = pOutput.context()->sectBegin(); section != sectEnd; ++section) {
    strcpy((char*)(data + shstrsize), (*section)->name().c_str());
    shstrsize += (*section)->name().size() + 1;
  }

  shstrtab.setKind(LDFileFormat::NamePool);
  shstrtab.setType(llvm::ELF::SHT_STRTAB);
  shstrtab.setFlag(0x0);
  shstrtab.setAddr(0x0);
}

/// emitSectionData
void
ELFWriter::emitSectionData(const Layout& pLayout,
                           const LDSection& pSection,
                           MemoryRegion& pRegion) const
{
  const SectionData* data = pSection.getSectionData();
  SectionData::const_iterator fragIter, fragEnd = data->end();
  size_t cur_offset = 0;
  for (fragIter = data->begin(); fragIter != fragEnd; ++fragIter) {
    size_t size = computeFragmentSize(pLayout, *fragIter);
    switch(fragIter->getKind()) {
      case Fragment::Region: {
        const RegionFragment& region_frag = llvm::cast<RegionFragment>(*fragIter);
        const uint8_t* from = region_frag.getRegion().start();
        memcpy(pRegion.getBuffer(cur_offset), from, size);
        break;
      }
      case Fragment::Alignment: {
        // TODO: emit values with different sizes (> 1 byte), and emit nops
        AlignFragment& align_frag = llvm::cast<AlignFragment>(*fragIter);
        uint64_t count = size / align_frag.getValueSize();
        switch (align_frag.getValueSize()) {
          case 1u:
            std::memset(pRegion.getBuffer(cur_offset),
                        align_frag.getValue(),
                        count);
            break;
          default:
            llvm::report_fatal_error("unsupported value size for align fragment emission yet.\n");
            break;
        }
        break;
      }
      case Fragment::Fillment: {
        FillFragment& fill_frag = llvm::cast<FillFragment>(*fragIter);
        if (0 == size ||
            0 == fill_frag.getValueSize() ||
            0 == fill_frag.getSize()) {
          // ignore virtual fillment
          break;
        }

        uint64_t num_tiles = fill_frag.getSize() / fill_frag.getValueSize();
        for (uint64_t i = 0; i != num_tiles; ++i) {
          std::memset(pRegion.getBuffer(cur_offset),
                      fill_frag.getValue(),
                      fill_frag.getValueSize());
        }
        break;
      }
      case Fragment::Relocation:
        llvm::report_fatal_error("relocation fragment should not be in a regular section.\n");
        break;
      case Fragment::Target:
        llvm::report_fatal_error("Target fragment should not be in a regular section.\n");
        break;
      default:
        llvm::report_fatal_error("invalid fragment should not be in a regular section.\n");
        break;
    }
    cur_offset += size;
  }
}

/// emitRelocation
void ELFWriter::emitRelocation(const Layout& pLayout,
                               const Output& pOutput,
                               const LDSection& pSection,
                               MemoryRegion& pRegion) const
{
  const SectionData* sect_data = pSection.getSectionData();
  assert(NULL != sect_data && "SectionData is NULL in emitRelocation!");

  if (pSection.type() == SHT_REL)
    emitRel(pLayout, pOutput, *sect_data, pRegion);
  else if (pSection.type() == SHT_RELA)
    emitRela(pLayout, pOutput, *sect_data, pRegion);
  else
    llvm::report_fatal_error("unsupported relocation section type!");
}


/// emitRel
void ELFWriter::emitRel(const Layout& pLayout,
                        const Output& pOutput,
                        const SectionData& pSectionData,
                        MemoryRegion& pRegion) const
{
  Elf32_Rel* rel = reinterpret_cast<Elf32_Rel*>(pRegion.start());

  Relocation* relocation = 0;
  FragmentRef* frag_ref = 0;

  for (SectionData::const_iterator it = pSectionData.begin(),
       ie = pSectionData.end(); it != ie; ++it, ++rel) {

    relocation = &(llvm::cast<Relocation>(*it));
    frag_ref = &(relocation->targetRef());

    if(pOutput.type() == Output::DynObj || pOutput.type() == Output::Exec) {
      rel->r_offset = static_cast<Elf32_Addr>(
                      frag_ref->frag()->getParent()->getSection().addr() +
                      pLayout.getOutputOffset(*frag_ref));
    }
    else {
      rel->r_offset = static_cast<Elf32_Addr>(
                      frag_ref->frag()->getParent()->getSection().offset() +
                      pLayout.getOutputOffset(*frag_ref));
    }

    Elf32_Word Index;
    if( relocation->symInfo() == NULL )
      Index = 0;
    else
      Index = static_cast<Elf32_Word>(
              f_Backend.getSymbolIdx(relocation->symInfo()->outSymbol()));

    rel->setSymbolAndType(Index, relocation->type());
  }
}

/// emitRela
void ELFWriter::emitRela(const Layout& pLayout,
                         const Output& pOutput,
                         const SectionData& pSectionData,
                         MemoryRegion& pRegion) const
{
  Elf32_Rela* rel = reinterpret_cast<Elf32_Rela*>(pRegion.start());

  Relocation* relocation = 0;
  FragmentRef* frag_ref = 0;

  for (SectionData::const_iterator it = pSectionData.begin(),
       ie = pSectionData.end(); it != ie; ++it, ++rel) {

    relocation = &(llvm::cast<Relocation>(*it));
    frag_ref = &(relocation->targetRef());

    if(pOutput.type() == Output::DynObj || pOutput.type() == Output::Exec) {
      rel->r_offset = static_cast<Elf32_Addr>(
                      frag_ref->frag()->getParent()->getSection().addr() +
                      pLayout.getOutputOffset(*frag_ref));
    }
    else {
      rel->r_offset = static_cast<Elf32_Addr>(
                      frag_ref->frag()->getParent()->getSection().offset() +
                      pLayout.getOutputOffset(*frag_ref));
    }

    Elf32_Word Index;
    if( relocation->symInfo() == NULL )
      Index = 0;
    else
      Index = static_cast<Elf32_Word>(
              f_Backend.getSymbolIdx(relocation->symInfo()->outSymbol()));

    rel->setSymbolAndType(Index, relocation->type());
    rel->r_addend = relocation->addend();
  }
}

/// getELF32SectEntrySize - compute Elf32_Shdr::sh_entsize
uint64_t ELFWriter::getELF32SectEntrySize(const LDSection& pSection) const
{
  if (llvm::ELF::SHT_DYNSYM == pSection.type() ||
      llvm::ELF::SHT_SYMTAB == pSection.type())
    return sizeof(llvm::ELF::Elf32_Sym);
  if (llvm::ELF::SHT_REL == pSection.type())
    return sizeof(llvm::ELF::Elf32_Rel);
  if (llvm::ELF::SHT_RELA == pSection.type())
    return sizeof(llvm::ELF::Elf32_Rela);
  if (llvm::ELF::SHT_HASH == pSection.type())
    return sizeof(llvm::ELF::Elf32_Word);
  if (llvm::ELF::SHT_DYNAMIC == pSection.type())
    return sizeof(llvm::ELF::Elf32_Dyn);
  return 0x0;
}

/// getELF64SectEntrySize - compute Elf64_Shdr::sh_entsize
uint64_t ELFWriter::getELF64SectEntrySize(const LDSection& pSection) const
{
  if (llvm::ELF::SHT_DYNSYM == pSection.type() ||
      llvm::ELF::SHT_SYMTAB == pSection.type())
    return sizeof(llvm::ELF::Elf64_Sym);
  if (llvm::ELF::SHT_REL == pSection.type())
    return sizeof(llvm::ELF::Elf64_Rel);
  if (llvm::ELF::SHT_RELA == pSection.type())
    return sizeof(llvm::ELF::Elf64_Rela);
  if (llvm::ELF::SHT_HASH == pSection.type())
    return sizeof(llvm::ELF::Elf64_Word);
  if (llvm::ELF::SHT_DYNAMIC == pSection.type())
    return sizeof(llvm::ELF::Elf64_Dyn);
  return 0x0;
}

/// getSectLink - compute ElfXX_Shdr::sh_link
uint64_t ELFWriter::getSectLink(const LDSection& pSection, const Output& pOutput) const
{
  const LDContext* context = pOutput.context();
  if (llvm::ELF::SHT_SYMTAB == pSection.type())
    return context->getSectionIdx(".strtab");
  if (llvm::ELF::SHT_DYNSYM == pSection.type())
    return context->getSectionIdx(".dynstr");
  if (llvm::ELF::SHT_DYNAMIC == pSection.type())
    return context->getSectionIdx(".dynstr");
  if (llvm::ELF::SHT_HASH == pSection.type())
    return context->getSectionIdx(".dynsym");
  if (llvm::ELF::SHT_REL == pSection.type() ||
      llvm::ELF::SHT_RELA == pSection.type()) {
    if (pOutput.type() == Output::Object)
      return context->getSectionIdx(".symtab");
    else
      return context->getSectionIdx(".dynsym");
  }
  return llvm::ELF::SHN_UNDEF;
}

/// getSectInfo - compute ElfXX_Shdr::sh_info
uint64_t ELFWriter::getSectInfo(const LDSection& pSection, const Output& pOutput) const
{
  const LDSection* info_link = pSection.getLink();
  if (NULL == info_link)
    return 0x0;
  return info_link->index();
}

/// getELF32LastStartOffset
uint64_t ELFWriter::getELF32LastStartOffset(const Output& pOutput) const
{
  LDSection* lastSect = pOutput.context()->getSectionTable().back();
  assert(lastSect != NULL);
  return Align<32>(lastSect->offset() + lastSect->size());
}

/// getELF64LastStartOffset
uint64_t ELFWriter::getELF64LastStartOffset(const Output& pOutput) const
{
  LDSection* lastSect = pOutput.context()->getSectionTable().back();
  assert(lastSect != NULL);
  return Align<64>(lastSect->offset() + lastSect->size());
}

