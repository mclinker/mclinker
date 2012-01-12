//===- ELFReader.tcc ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file is the template implemenation of ELFReaders
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ELFReader<32, true>
#include <cstring>
#include <vector>

/// constructor
ELFReader<32, true>::ELFReader(GNULDBackend& pBackend)
  : ELFReaderIF(pBackend) {
}

/// destructor
ELFReader<32, true>::~ELFReader()
{
}

/// isELF - is this a ELF file
bool ELFReader<32, true>::isELF(void* pELFHeader) const
{
  llvm::ELF::Elf32_Ehdr* hdr =
                          reinterpret_cast<llvm::ELF::Elf32_Ehdr*>(pELFHeader);
  if (0 == memcmp(llvm::ELF::ElfMagic, hdr, 4))
    return true;
  return false;
}

/// isMyEndian - is this ELF file in the same endian to me?
bool ELFReader<32, true>::isMyEndian(void* pELFHeader) const
{
  llvm::ELF::Elf32_Ehdr* hdr =
                          reinterpret_cast<llvm::ELF::Elf32_Ehdr*>(pELFHeader);

  return (hdr->e_ident[llvm::ELF::EI_DATA] == llvm::ELF::ELFDATA2LSB);
}

/// isMyMachine - is this ELF file generated for the same machine.
bool ELFReader<32, true>::isMyMachine(void* pELFHeader) const
{
  llvm::ELF::Elf32_Ehdr* hdr =
                          reinterpret_cast<llvm::ELF::Elf32_Ehdr*>(pELFHeader);

  if (llvm::sys::isLittleEndianHost())
    return (hdr->e_machine == target().machine());
  return (bswap16(hdr->e_machine) == target().machine());
}

/// fileType - return the file type
MCLDFile::Type ELFReader<32, true>::fileType(void* pELFHeader) const
{
  llvm::ELF::Elf32_Ehdr* hdr =
                          reinterpret_cast<llvm::ELF::Elf32_Ehdr*>(pELFHeader);
  uint32_t type = 0x0;
  if (llvm::sys::isLittleEndianHost())
    type = hdr->e_type;
  else
    type = bswap16(hdr->e_type);

  switch(type) {
  case llvm::ELF::ET_REL:
    return MCLDFile::Object;
  case llvm::ELF::ET_EXEC:
    return MCLDFile::Exec;
  case llvm::ELF::ET_DYN:
    return MCLDFile::DynObj;
  case llvm::ELF::ET_CORE:
    return MCLDFile::CoreFile;
  case llvm::ELF::ET_NONE:
  default:
    return MCLDFile::Unknown;
  }
}

/// readSectionHeaders - read ELF section header table and create LDSections
bool ELFReader<32, true>::readSectionHeaders(Input& pInput,
                                             MCLinker& pLinker,
                                             void* pELFHeader) const
{
  llvm::ELF::Elf32_Ehdr* ehdr =
                          reinterpret_cast<llvm::ELF::Elf32_Ehdr*>(pELFHeader);

  uint32_t shoff     = 0x0;
  uint16_t shentsize = 0x0;
  uint16_t shnum     = 0x0;
  uint16_t shstrtab  = 0x0;

  if (llvm::sys::isLittleEndianHost()) {
    shoff     = ehdr->e_shoff;
    shentsize = ehdr->e_shentsize;
    shnum     = ehdr->e_shnum;
    shstrtab  = ehdr->e_shstrndx;
  }
  else {
    shoff     = bswap32(ehdr->e_shoff);
    shentsize = bswap16(ehdr->e_shentsize);
    shnum     = bswap16(ehdr->e_shnum);
    shstrtab  = bswap16(ehdr->e_shstrndx);
  }

  // If the file has no section header table, e_shoff holds zero.
  if (0x0 == shoff)
    return true;

  MemoryRegion* shdr_region = pInput.memArea()->request(shoff, shnum*shentsize);
  llvm::ELF::Elf32_Shdr* shdrTab =
                reinterpret_cast<llvm::ELF::Elf32_Shdr*>(shdr_region->start());

  uint32_t sh_name   = 0x0;
  uint32_t sh_type   = 0x0;
  uint32_t sh_flags  = 0x0;
  uint32_t sh_offset = 0x0;
  uint32_t sh_size   = 0x0;
  uint32_t sh_link   = 0x0;
  uint32_t sh_info   = 0x0;

  // get .shstrtab first
  llvm::ELF::Elf32_Shdr* shdr = &shdrTab[shstrtab];
  if (llvm::sys::isLittleEndianHost()) {
    sh_offset = shdr->sh_offset;
    sh_size   = shdr->sh_size;
  }
  else {
    sh_offset = bswap32(shdr->sh_offset);
    sh_size   = bswap32(shdr->sh_size);
  }
  const char* sect_name = reinterpret_cast<const char*>(
                       pInput.memArea()->request(sh_offset, sh_size)->start());

  LinkInfoList link_info_list;

  // create all LDSections
  for (size_t idx = 0; idx < shnum; ++idx) {
    if (llvm::sys::isLittleEndianHost()) {
      sh_name   = shdrTab[idx].sh_name;
      sh_type   = shdrTab[idx].sh_type;
      sh_flags  = shdrTab[idx].sh_flags;
      sh_offset = shdrTab[idx].sh_offset;
      sh_size   = shdrTab[idx].sh_size;
      sh_link   = shdrTab[idx].sh_link;
      sh_info   = shdrTab[idx].sh_info;
    }
    else {
      sh_name   = bswap32(shdrTab[idx].sh_name);
      sh_type   = bswap32(shdrTab[idx].sh_type);
      sh_flags  = bswap32(shdrTab[idx].sh_flags);
      sh_offset = bswap32(shdrTab[idx].sh_offset);
      sh_size   = bswap32(shdrTab[idx].sh_size);
      sh_link   = bswap32(shdrTab[idx].sh_link);
      sh_info   = bswap32(shdrTab[idx].sh_info);
    }

    LDFileFormat::Kind kind = getLDSectionKind(sh_type,
                                               sect_name+sh_name);

    LDSection& section = pLinker.createSectHdr(sect_name+sh_name,
                                               kind,
                                               sh_type,
                                               sh_flags);

    section.setSize(sh_size);
    section.setOffset(sh_offset);
    section.setIndex(pInput.context()->numOfSections());

    if (sh_link != 0x0 || sh_info != 0x0) {
      LinkInfo link_info = { &section, sh_link, sh_info };
      link_info_list.push_back(link_info);
    }

    pInput.context()->getSectionTable().push_back(&section);
  }

  // set up InfoLink
  LinkInfoList::iterator info, infoEnd = link_info_list.end();
  for (info = link_info_list.begin(); info != infoEnd; ++info) {
    if (LDFileFormat::NamePool == info->section->kind()) {
      info->section->setLinkInfo(pInput.context()->getSection(info->sh_link));
      continue;
    }
    if (LDFileFormat::Relocation == info->section->kind()) {
      info->section->setLinkInfo(pInput.context()->getSection(info->sh_info));
      continue;
    }
  }

  return true;
}

/// readSymbols - read ELF symbols and create LDSymbol
bool ELFReader<32, true>::readSymbols(Input& pInput,
                                      MCLinker& pLinker,
                                      const MemoryRegion& pRegion,
                                      const char* pStrTab) const
{
  // get number of symbols
  size_t entsize = pRegion.size()/sizeof(llvm::ELF::Elf32_Sym);
  llvm::ELF::Elf32_Sym* symtab =
                      reinterpret_cast<llvm::ELF::Elf32_Sym*>(pRegion.start());

  uint32_t st_name  = 0x0;
  uint32_t st_value = 0x0;
  uint32_t st_size  = 0x0;
  uint8_t  st_info  = 0x0;
  uint8_t  st_other = 0x0;
  uint16_t st_shndx = 0x0;
  // skip the first NULL symbol
  pInput.context()->symtab().push_back(NULL);

  for (size_t idx = 1; idx < entsize; ++idx) {
    st_info  = symtab[idx].st_info;
    st_other = symtab[idx].st_other;
    if (llvm::sys::isLittleEndianHost()) {
      st_name  = symtab[idx].st_name;
      st_value = symtab[idx].st_value;
      st_size  = symtab[idx].st_size;
      st_shndx = symtab[idx].st_shndx;
    }
    else {
      st_name  = bswap32(symtab[idx].st_name);
      st_value = bswap32(symtab[idx].st_value);
      st_size  = bswap32(symtab[idx].st_size);
      st_shndx = bswap16(symtab[idx].st_shndx);
    }

    // get ld_name
    llvm::StringRef ld_name(pStrTab + st_name);

    // get ld_type
    ResolveInfo::Type ld_type = static_cast<ResolveInfo::Type>(st_info & 0xF);

    // get ld_desc
    ResolveInfo::Desc ld_desc = getSymDesc(st_shndx, pInput);

    // get ld_binding
    ResolveInfo::Binding ld_binding = getSymBinding((st_info >> 4), st_shndx, st_other);

    // get ld_value - ld_value must be section relative.
    uint64_t ld_value = getSymValue(st_value, st_shndx, pInput);

    // get the input fragment
    MCFragmentRef* ld_frag_ref = getSymFragmentRef(pInput,
                                                   pLinker,
                                                   st_shndx,
                                                   ld_value);

    // get ld_vis
    ResolveInfo::Visibility ld_vis = getSymVisibility(st_other);

    // push into MCLinker
    LDSymbol* input_sym = NULL;

    if (pInput.type() == Input::Object) {
      input_sym = pLinker.addSymbol<Input::Object>(ld_name,
                                                   ld_type,
                                                   ld_desc,
                                                   ld_binding,
                                                   st_size,
                                                   ld_value,
                                                   ld_frag_ref,
                                                   ld_vis);
      // push into the input file
      pInput.context()->symtab().push_back(input_sym);
      continue;
    }
    else if (pInput.type() == Input::DynObj) {
      input_sym = pLinker.addSymbol<Input::DynObj>(ld_name,
                                                   ld_type,
                                                   ld_desc,
                                                   ld_binding,
                                                   st_size,
                                                   ld_value,
                                                   ld_frag_ref,
                                                   ld_vis);
      continue;
    }

  } // end of for loop
  return true;
}

/// readRela - read ELF rela and create Relocation
bool ELFReader<32, true>::readRela(Input& pInput,
                                   MCLinker& pLinker,
                                   LDSection& pSection,
                                   const MemoryRegion& pRegion) const
{
  // get the number of rela
  size_t entsize = pRegion.size() / sizeof(llvm::ELF::Elf32_Rela);
  llvm::ELF::Elf32_Rela* relaTab =
                     reinterpret_cast<llvm::ELF::Elf32_Rela*>(pRegion.start());

  for (size_t idx=0; idx < entsize; ++idx) {
    uint32_t r_offset = 0x0;
    uint32_t r_info   = 0x0;
    int32_t  r_addend = 0;
    if (llvm::sys::isLittleEndianHost()) {
      r_offset = relaTab[idx].r_offset;
      r_info   = relaTab[idx].r_info;
      r_addend = relaTab[idx].r_addend;
    }
    else {
      r_offset = bswap32(relaTab[idx].r_offset);
      r_info   = bswap32(relaTab[idx].r_info);
      r_addend = bswap32(relaTab[idx].r_addend);
    }

    uint8_t  r_type = static_cast<unsigned char>(r_info);
    uint32_t r_sym  = (r_info >> 8);
    LDSymbol* symbol = pInput.context()->getSymbol(r_sym);
    if (NULL == symbol) {
      llvm::report_fatal_error(llvm::Twine("invalid symbol index :") +
                               llvm::Twine(r_sym) +
                               llvm::Twine(" in file `") +
                               pInput.path().native() +
                               llvm::Twine("'.\n"));
    }

    ResolveInfo* resolve_info = symbol->resolveInfo();

    MCFragmentRef* frag_ref =
         pLinker.getLayout().getFragmentRef(*pSection.getLinkInfo(), r_offset);

    if (NULL == frag_ref) {
      llvm::report_fatal_error(llvm::Twine("invalid sh_info: ") +
                               llvm::Twine(pSection.getLinkInfo()->index()) +
                               llvm::Twine(" of the relocation section `") +
                               pSection.name() +
                               llvm::Twine("' in file `") +
                               pInput.path().native() +
                               llvm::Twine(".\n"));
    }

    pLinker.addRelocation(r_type, *resolve_info, *frag_ref, r_addend);
  }
  return true;
}

/// readRel - read ELF rel and create Relocation
bool ELFReader<32, true>::readRel(Input& pInput,
                                  MCLinker& pLinker,
                                  LDSection& pSection,
                                  const MemoryRegion& pRegion) const
{
  // get the number of rel
  size_t entsize = pRegion.size() / sizeof(llvm::ELF::Elf32_Rel);
  llvm::ELF::Elf32_Rel* relTab =
                      reinterpret_cast<llvm::ELF::Elf32_Rel*>(pRegion.start());

  for (size_t idx=0; idx < entsize; ++idx) {
    uint32_t r_offset = 0x0;
    uint32_t r_info   = 0x0;
    if (llvm::sys::isLittleEndianHost()) {
      r_offset = relTab[idx].r_offset;
      r_info   = relTab[idx].r_info;
    }
    else {
      r_offset = bswap32(relTab[idx].r_offset);
      r_info   = bswap32(relTab[idx].r_info);
    }

    uint8_t  r_type = static_cast<unsigned char>(r_info);
    uint32_t r_sym  = (r_info >> 8);

    LDSymbol* symbol = pInput.context()->getSymbol(r_sym);
    if (NULL == symbol) {
      llvm::report_fatal_error(llvm::Twine("invalid symbol index :") +
                               llvm::Twine(r_sym) +
                               llvm::Twine(" in file `") +
                               pInput.path().native() +
                               llvm::Twine("'.\n"));
    }

    ResolveInfo* resolve_info = symbol->resolveInfo();

    MCFragmentRef* frag_ref =
         pLinker.getLayout().getFragmentRef(*pSection.getLinkInfo(), r_offset);

    if (NULL == frag_ref) {
      llvm::report_fatal_error(llvm::Twine("invalid sh_info: ") +
                               llvm::Twine(pSection.getLinkInfo()->index()) +
                               llvm::Twine(" of the relocation section `") +
                               pSection.name() +
                               llvm::Twine("' in file `") +
                               pInput.path().native() +
                               llvm::Twine(".\n"));
    }

    pLinker.addRelocation(r_type, *resolve_info, *frag_ref);
  }
  return true;
}

