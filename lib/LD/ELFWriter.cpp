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

#include <mcld/LinkerConfig.h>
#include <mcld/Module.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Fragment/AlignFragment.h>
#include <mcld/Fragment/FillFragment.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/Fragment/Stub.h>
#include <mcld/Fragment/NullFragment.h>
#include <mcld/LD/ELFWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/ELFSegment.h>
#include <mcld/LD/ELFSegmentFactory.h>
#include <mcld/LD/RelocData.h>
#include <mcld/LD/EhFrame.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>

using namespace llvm::ELF;
using namespace mcld;

/// writeELF32Header - write ELF header
void ELFWriter::writeELF32Header(const LinkerConfig& pConfig,
                                 const Module& pModule,
                                 MemoryArea& pOutput) const
{
  // ELF header must start from 0x0
  MemoryRegion *region = pOutput.request(0, sizeof(Elf32_Ehdr));
  Elf32_Ehdr* header = (Elf32_Ehdr*)region->start();

  memcpy(header->e_ident, ElfMagic, EI_MAG3+1);

  header->e_ident[EI_CLASS]      = ELFCLASS32;
  header->e_ident[EI_DATA]       = pConfig.targets().isLittleEndian()?
                                       ELFDATA2LSB : ELFDATA2MSB;
  header->e_ident[EI_VERSION]    = target().getInfo().ELFVersion();
  header->e_ident[EI_OSABI]      = target().getInfo().OSABI();
  header->e_ident[EI_ABIVERSION] = target().getInfo().ABIVersion();

  // FIXME: add processor-specific and core file types.
  switch(pConfig.codeGenType()) {
    case LinkerConfig::Object:
      header->e_type = ET_REL;
      break;
    case LinkerConfig::DynObj:
      header->e_type = ET_DYN;
      break;
    case LinkerConfig::Exec:
      header->e_type = ET_EXEC;
      break;
    default:
      llvm::errs() << "unspported output file type: " << pConfig.codeGenType() << ".\n";
      header->e_type = ET_NONE;
  }
  header->e_machine   = target().getInfo().machine();
  header->e_version   = header->e_ident[EI_VERSION];
  header->e_entry     = getEntryPoint(pConfig, pModule);

  if (LinkerConfig::Object != pConfig.codeGenType())
    header->e_phoff   = sizeof(Elf32_Ehdr);
  else
    header->e_phoff   = 0x0;

  header->e_shoff     = getELF32LastStartOffset(pModule);
  header->e_flags     = target().flags();
  header->e_ehsize    = sizeof(Elf32_Ehdr);
  header->e_phentsize = sizeof(Elf32_Phdr);
  header->e_phnum     = target().numOfSegments();
  header->e_shentsize = sizeof(Elf32_Shdr);
  header->e_shnum     = pModule.size();
  header->e_shstrndx  = pModule.getSection(".shstrtab")->index();
}

/// writeELF64Header - write ELF header
void ELFWriter::writeELF64Header(const LinkerConfig& pConfig,
                                 const Module& pModule,
                                 MemoryArea& pOutput) const
{
  // ELF header must start from 0x0
  MemoryRegion *region = pOutput.request(0, sizeof(Elf64_Ehdr));
  Elf64_Ehdr* header = (Elf64_Ehdr*)region->start();

  memcpy(header->e_ident, ElfMagic, EI_MAG3+1);

  header->e_ident[EI_CLASS]      = ELFCLASS64;
  header->e_ident[EI_DATA]       = pConfig.targets().isLittleEndian()?
                                       ELFDATA2LSB : ELFDATA2MSB;
  header->e_ident[EI_VERSION]    = target().getInfo().ELFVersion();
  header->e_ident[EI_OSABI]      = target().getInfo().OSABI();
  header->e_ident[EI_ABIVERSION] = target().getInfo().ABIVersion();

  // FIXME: add processor-specific and core file types.
  switch(pConfig.codeGenType()) {
    case LinkerConfig::Object:
      header->e_type = ET_REL;
      break;
    case LinkerConfig::DynObj:
      header->e_type = ET_DYN;
      break;
    case LinkerConfig::Exec:
      header->e_type = ET_EXEC;
      break;
    default:
      llvm::errs() << "unspported output file type: " << pConfig.codeGenType() << ".\n";
      header->e_type = ET_NONE;
  }
  header->e_machine   = target().getInfo().machine();
  header->e_version   = header->e_ident[EI_VERSION];
  header->e_entry     = getEntryPoint(pConfig, pModule);

  if (LinkerConfig::Object != pConfig.codeGenType())
    header->e_phoff   = sizeof(Elf64_Ehdr);
  else
    header->e_phoff   = 0x0;

  header->e_shoff     = getELF64LastStartOffset(pModule);
  header->e_flags     = target().flags();
  header->e_ehsize    = sizeof(Elf64_Ehdr);
  header->e_phentsize = sizeof(Elf64_Phdr);
  header->e_phnum     = target().numOfSegments();
  header->e_shentsize = sizeof(Elf64_Shdr);
  header->e_shnum     = pModule.size();
  header->e_shstrndx  = pModule.getSection(".shstrtab")->index();
}

/// getEntryPoint
uint64_t ELFWriter::getEntryPoint(const LinkerConfig& pConfig,
                                  const Module& pModule) const
{

  llvm::StringRef entry_name;
  if (pConfig.options().hasEntry())
    entry_name = pConfig.options().entry();
  else
    entry_name = target().entry();

  uint64_t result = 0x0;

  bool issue_warning = (pConfig.options().hasEntry() &&
                        LinkerConfig::Object != pConfig.codeGenType() &&
                        LinkerConfig::DynObj != pConfig.codeGenType());

  const LDSymbol* entry_symbol = pModule.getNamePool().findSymbol(entry_name);

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
ELFWriter::emitELF32SectionHeader(const Module& pModule,
                                  const LinkerConfig& pConfig,
                                  MemoryArea& pOutput) const
{
  // emit section header
  unsigned int sectNum = pModule.size();
  unsigned int header_size = sizeof(Elf32_Shdr) * sectNum;
  MemoryRegion* region = pOutput.request(getELF32LastStartOffset(pModule), header_size);
  Elf32_Shdr* shdr = (Elf32_Shdr*)region->start();

  // Iterate the SectionTable in LDContext
  unsigned int sectIdx = 0;
  unsigned int shstridx = 0; // NULL section has empty name
  for (; sectIdx < sectNum; ++sectIdx) {
    const LDSection *ld_sect   = pModule.getSectionTable().at(sectIdx);
    shdr[sectIdx].sh_name      = shstridx;
    shdr[sectIdx].sh_type      = ld_sect->type();
    shdr[sectIdx].sh_flags     = ld_sect->flag();
    shdr[sectIdx].sh_addr      = ld_sect->addr();
    shdr[sectIdx].sh_offset    = ld_sect->offset();
    shdr[sectIdx].sh_size      = ld_sect->size();
    shdr[sectIdx].sh_addralign = ld_sect->align();
    shdr[sectIdx].sh_entsize   = getELF32SectEntrySize(*ld_sect);
    shdr[sectIdx].sh_link      = getSectLink(*ld_sect, pConfig);
    shdr[sectIdx].sh_info      = getSectInfo(*ld_sect);

    // adjust strshidx
    shstridx += ld_sect->name().size() + 1;
  }
}

/// emitELF64SectionHeader - emit Elf64_Shdr
void
ELFWriter::emitELF64SectionHeader(const Module& pModule,
                                  const LinkerConfig& pConfig,
                                  MemoryArea& pOutput) const
{
  // emit section header
  unsigned int sectNum = pModule.size();
  unsigned int header_size = sizeof(Elf64_Shdr) * sectNum;
  MemoryRegion* region = pOutput.request(getELF64LastStartOffset(pModule),
                                         header_size);
  Elf64_Shdr* shdr = (Elf64_Shdr*)region->start();

  // Iterate the SectionTable in LDContext
  unsigned int sectIdx = 0;
  unsigned int shstridx = 0; // NULL section has empty name
  for (; sectIdx < sectNum; ++sectIdx) {
    const LDSection *ld_sect   = pModule.getSectionTable().at(sectIdx);
    shdr[sectIdx].sh_name      = shstridx;
    shdr[sectIdx].sh_type      = ld_sect->type();
    shdr[sectIdx].sh_flags     = ld_sect->flag();
    shdr[sectIdx].sh_addr      = ld_sect->addr();
    shdr[sectIdx].sh_offset    = ld_sect->offset();
    shdr[sectIdx].sh_size      = ld_sect->size();
    shdr[sectIdx].sh_addralign = ld_sect->align();
    shdr[sectIdx].sh_entsize   = getELF64SectEntrySize(*ld_sect);
    shdr[sectIdx].sh_link      = getSectLink(*ld_sect, pConfig);
    shdr[sectIdx].sh_info      = getSectInfo(*ld_sect);

    // adjust strshidx
    shstridx += ld_sect->name().size() + 1;
  }
}


/// emitELF32ProgramHeader - emit Elf32_Phdr
void ELFWriter::emitELF32ProgramHeader(MemoryArea& pOutput) const
{
  uint64_t start_offset, phdr_size;

  start_offset = sizeof(Elf32_Ehdr);
  phdr_size = sizeof(Elf32_Phdr);
  // Program header must start directly after ELF header
  MemoryRegion *region = pOutput.request(start_offset,
                                         target().numOfSegments() * phdr_size);

  Elf32_Phdr* phdr = (Elf32_Phdr*)region->start();

  // Iterate the elf segment table in GNULDBackend
  size_t index = 0;
  ELFSegmentFactory::const_iterator seg = target().elfSegmentTable().begin(),
                                 segEnd = target().elfSegmentTable().end();
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
void ELFWriter::emitELF64ProgramHeader(MemoryArea& pOutput) const
{
  uint64_t start_offset, phdr_size;

  start_offset = sizeof(Elf64_Ehdr);
  phdr_size = sizeof(Elf64_Phdr);
  // Program header must start directly after ELF header
  MemoryRegion *region = pOutput.request(start_offset,
                                         target().numOfSegments() * phdr_size);
  Elf64_Phdr* phdr = (Elf64_Phdr*)region->start();

  // Iterate the elf segment table in GNULDBackend
  size_t index = 0;
  ELFSegmentFactory::const_iterator seg = target().elfSegmentTable().begin(),
                                 segEnd = target().elfSegmentTable().end();
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

/// emitELFShStrTab - emit section string table
void
ELFWriter::emitELFShStrTab(const LDSection& pShStrTab, const Module& pModule,
                           MemoryArea& pOutput)
{
  // write out data
  MemoryRegion* region = pOutput.request(pShStrTab.offset(), pShStrTab.size());
  unsigned char* data = region->start();
  size_t shstrsize = 0;
  Module::const_iterator section, sectEnd = pModule.end();
  for (section = pModule.begin(); section != sectEnd; ++section) {
    strcpy((char*)(data + shstrsize), (*section)->name().data());
    shstrsize += (*section)->name().size() + 1;
  }
}

/// emitSectionData
void
ELFWriter::emitSectionData(const LDSection& pSection,
                           MemoryRegion& pRegion) const
{
  const SectionData* sd = NULL;
  switch (pSection.kind()) {
    case LDFileFormat::Relocation:
      return;
    case LDFileFormat::EhFrame:
      sd = &pSection.getEhFrame()->getSectionData();
      break;
    default:
      sd = pSection.getSectionData();
      break;
  }
  emitSectionData(*sd, pRegion);
}

/// emitRelocation
void ELFWriter::emitRelocation(const LinkerConfig& pConfig,
                               const LDSection& pSection,
                               MemoryRegion& pRegion) const
{
  const RelocData* sect_data = pSection.getRelocData();
  assert(NULL != sect_data && "SectionData is NULL in emitRelocation!");

  if (pSection.type() == SHT_REL)
    emitRel(pConfig, *sect_data, pRegion);
  else if (pSection.type() == SHT_RELA)
    emitRela(pConfig, *sect_data, pRegion);
  else
    llvm::report_fatal_error("unsupported relocation section type!");
}


/// emitRel
void ELFWriter::emitRel(const LinkerConfig& pConfig,
                        const RelocData& pRelocData,
                        MemoryRegion& pRegion) const
{
  Elf32_Rel* rel = reinterpret_cast<Elf32_Rel*>(pRegion.start());

  Relocation* relocation = 0;
  FragmentRef* frag_ref = 0;

  for (RelocData::const_iterator it = pRelocData.begin(),
       ie = pRelocData.end(); it != ie; ++it, ++rel) {

    relocation = &(llvm::cast<Relocation>(*it));
    frag_ref = &(relocation->targetRef());

    if(LinkerConfig::DynObj == pConfig.codeGenType() ||
       LinkerConfig::Exec == pConfig.codeGenType()) {
      rel->r_offset = static_cast<Elf32_Addr>(
                      frag_ref->frag()->getParent()->getSection().addr() +
                      frag_ref->getOutputOffset());
    }
    else {
      rel->r_offset = static_cast<Elf32_Addr>(frag_ref->getOutputOffset());
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
void ELFWriter::emitRela(const LinkerConfig& pConfig,
                         const RelocData& pRelocData,
                         MemoryRegion& pRegion) const
{
  Elf32_Rela* rel = reinterpret_cast<Elf32_Rela*>(pRegion.start());

  Relocation* relocation = 0;
  FragmentRef* frag_ref = 0;

  for (RelocData::const_iterator it = pRelocData.begin(),
       ie = pRelocData.end(); it != ie; ++it, ++rel) {

    relocation = &(llvm::cast<Relocation>(*it));
    frag_ref = &(relocation->targetRef());

    if(LinkerConfig::DynObj == pConfig.codeGenType() ||
       LinkerConfig::Exec == pConfig.codeGenType()) {
      rel->r_offset = static_cast<Elf32_Addr>(
                      frag_ref->frag()->getParent()->getSection().addr() +
                      frag_ref->getOutputOffset());
    }
    else {
      rel->r_offset = static_cast<Elf32_Addr>(frag_ref->getOutputOffset());
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
uint64_t ELFWriter::getSectLink(const LDSection& pSection,
                                const LinkerConfig& pConfig) const
{
  if (llvm::ELF::SHT_SYMTAB == pSection.type())
    return target().getOutputFormat()->getStrTab().index();
  if (llvm::ELF::SHT_DYNSYM == pSection.type())
    return target().getOutputFormat()->getDynStrTab().index();
  if (llvm::ELF::SHT_DYNAMIC == pSection.type())
    return target().getOutputFormat()->getDynStrTab().index();
  if (llvm::ELF::SHT_HASH == pSection.type())
    return target().getOutputFormat()->getDynSymTab().index();
  if (llvm::ELF::SHT_REL == pSection.type() ||
      llvm::ELF::SHT_RELA == pSection.type()) {
    if (LinkerConfig::Object == pConfig.codeGenType())
      return target().getOutputFormat()->getSymTab().index();
    else
      return target().getOutputFormat()->getDynSymTab().index();
  }
  // FIXME: currently we link ARM_EXIDX section to output text section here
  if (llvm::ELF::SHT_ARM_EXIDX == pSection.type())
    return target().getOutputFormat()->getText().index();
  return llvm::ELF::SHN_UNDEF;
}

/// getSectInfo - compute ElfXX_Shdr::sh_info
uint64_t ELFWriter::getSectInfo(const LDSection& pSection) const
{
  if (llvm::ELF::SHT_SYMTAB == pSection.type() ||
      llvm::ELF::SHT_DYNSYM == pSection.type())
    return pSection.getInfo();

  if (llvm::ELF::SHT_REL == pSection.type() ||
      llvm::ELF::SHT_RELA == pSection.type()) {
    const LDSection* info_link = pSection.getLink();
    if (NULL != info_link)
      return info_link->index();
  }

  return 0x0;
}

/// getELF32LastStartOffset
uint64_t ELFWriter::getELF32LastStartOffset(const Module& pModule) const
{
  const LDSection* lastSect = pModule.back();
  assert(lastSect != NULL);
  return Align<32>(lastSect->offset() + lastSect->size());
}

/// getELF64LastStartOffset
uint64_t ELFWriter::getELF64LastStartOffset(const Module& pModule) const
{
  const LDSection* lastSect = pModule.back();
  assert(lastSect != NULL);
  return Align<64>(lastSect->offset() + lastSect->size());
}

/// emitSectionData
void
ELFWriter::emitSectionData(const SectionData& pSD, MemoryRegion& pRegion) const
{
  SectionData::const_iterator fragIter, fragEnd = pSD.end();
  size_t cur_offset = 0;
  for (fragIter = pSD.begin(); fragIter != fragEnd; ++fragIter) {
    size_t size = fragIter->size();
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
            0 == fill_frag.size()) {
          // ignore virtual fillment
          break;
        }

        uint64_t num_tiles = fill_frag.size() / fill_frag.getValueSize();
        for (uint64_t i = 0; i != num_tiles; ++i) {
          std::memset(pRegion.getBuffer(cur_offset),
                      fill_frag.getValue(),
                      fill_frag.getValueSize());
        }
        break;
      }
      case Fragment::Stub: {
        Stub& stub_frag = llvm::cast<Stub>(*fragIter);
        memcpy(pRegion.getBuffer(cur_offset), stub_frag.getContent(), size);
        break;
      }
      case Fragment::Null: {
        assert(0x0 == size);
        break;
      }
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

