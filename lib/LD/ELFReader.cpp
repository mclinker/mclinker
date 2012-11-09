//===- ELFReader.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFReader.h>

#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Object/ObjectBuilder.h>

#include <cstring>

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Host.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFReaderIF
//===----------------------------------------------------------------------===//
/// GetSectionKind
LDFileFormat::Kind
ELFReaderIF::GetSectionKind(uint32_t pType, const char* pName)
{
  // name rules
  llvm::StringRef name(pName);
  if (name.startswith(".debug") ||
      name.startswith(".zdebug") ||
      name.startswith(".gnu.linkonce.wi.") ||
      name.startswith(".line") ||
      name.startswith(".stab"))
    return LDFileFormat::Debug;
  if (name.startswith(".comment"))
    return LDFileFormat::MetaData;
  if (name.startswith(".interp") || name.startswith(".dynamic"))
    return LDFileFormat::Note;
  if (name.startswith(".eh_frame"))
    return LDFileFormat::EhFrame;
  if (name.startswith(".eh_frame_hdr"))
    return LDFileFormat::EhFrameHdr;
  if (name.startswith(".gcc_except_table"))
    return LDFileFormat::GCCExceptTable;
  if (name.startswith(".note.GNU-stack"))
    return LDFileFormat::StackNote;

  // type rules
  switch(pType) {
  case llvm::ELF::SHT_NULL:
    return LDFileFormat::Null;
  case llvm::ELF::SHT_INIT_ARRAY:
  case llvm::ELF::SHT_FINI_ARRAY:
  case llvm::ELF::SHT_PREINIT_ARRAY:
  case llvm::ELF::SHT_PROGBITS:
    return LDFileFormat::Regular;
  case llvm::ELF::SHT_SYMTAB:
  case llvm::ELF::SHT_DYNSYM:
  case llvm::ELF::SHT_STRTAB:
  case llvm::ELF::SHT_HASH:
  case llvm::ELF::SHT_DYNAMIC:
    return LDFileFormat::NamePool;
  case llvm::ELF::SHT_RELA:
  case llvm::ELF::SHT_REL:
    return LDFileFormat::Relocation;
  case llvm::ELF::SHT_NOBITS:
    return LDFileFormat::BSS;
  case llvm::ELF::SHT_NOTE:
    return LDFileFormat::Note;
  case llvm::ELF::SHT_GROUP:
    return LDFileFormat::Group;
  case llvm::ELF::SHT_GNU_versym:
  case llvm::ELF::SHT_GNU_verdef:
  case llvm::ELF::SHT_GNU_verneed:
    return LDFileFormat::Version;
  case llvm::ELF::SHT_SHLIB:
    return LDFileFormat::Target;
  default:
    if ((pType >= llvm::ELF::SHT_LOPROC && pType <= llvm::ELF::SHT_HIPROC) ||
        (pType >= llvm::ELF::SHT_LOOS && pType <= llvm::ELF::SHT_HIOS) ||
        (pType >= llvm::ELF::SHT_LOUSER && pType <= llvm::ELF::SHT_HIUSER))
      return LDFileFormat::Target;
    fatal(diag::err_unsupported_section) << pName << pType;
  }
  return LDFileFormat::MetaData;
}

/// getSymType
ResolveInfo::Type ELFReaderIF::getSymType(uint8_t pInfo, uint16_t pShndx) const
{
  ResolveInfo::Type result = static_cast<ResolveInfo::Type>(pInfo & 0xF);
  if (llvm::ELF::SHN_ABS == pShndx && ResolveInfo::Section == result) {
    // In Mips, __gp_disp is a special section symbol. Its name comes from
    // .strtab, not .shstrtab. However, it is unique. Only it is also a ABS
    // symbol. So here is a tricky to identify __gp_disp and convert it to
    // Object symbol.
    return ResolveInfo::Object;
  }

  return result;
}

/// getSymDesc
ResolveInfo::Desc ELFReaderIF::getSymDesc(uint16_t pShndx, const Input& pInput) const
{
  if (pShndx == llvm::ELF::SHN_UNDEF)
    return ResolveInfo::Undefined;

  if (pShndx < llvm::ELF::SHN_LORESERVE) {
    // an ELF symbol defined in a section which we are not including
    // must be treated as an Undefined.
    // @ref Google gold linker: symtab.cc: 1086
    if (NULL == pInput.context()->getSection(pShndx) ||
        LDFileFormat::Ignore == pInput.context()->getSection(pShndx)->kind())
      return ResolveInfo::Undefined;
    return ResolveInfo::Define;
  }

  if (pShndx == llvm::ELF::SHN_ABS)
    return ResolveInfo::Define;

  if (pShndx == llvm::ELF::SHN_COMMON)
    return ResolveInfo::Common;

  // FIXME: ELF weak alias should be ResolveInfo::Indirect
  return ResolveInfo::NoneDesc;
}

/// getSymBinding
ResolveInfo::Binding
ELFReaderIF::getSymBinding(uint8_t pBinding, uint16_t pShndx, uint8_t pVis) const
{

  // TODO:
  // if --just-symbols option is enabled, the symbol must covert to Absolute

  switch(pBinding) {
  case llvm::ELF::STB_LOCAL:
    return ResolveInfo::Local;
  case llvm::ELF::STB_GLOBAL:
    return ResolveInfo::Global;
  case llvm::ELF::STB_WEAK:
    return ResolveInfo::Weak;
  }

  if (pShndx == llvm::ELF::SHN_ABS)
    return ResolveInfo::Absolute;

  return ResolveInfo::NoneBinding;
}

/// getSymFragmentRef
FragmentRef*
ELFReaderIF::getSymFragmentRef(Input& pInput,
                               uint16_t pShndx,
                               uint32_t pOffset) const
{

  if (Input::DynObj == pInput.type())
    return FragmentRef::Null();

  if (pShndx == llvm::ELF::SHN_UNDEF || pShndx >= llvm::ELF::SHN_LORESERVE)
    return FragmentRef::Null();

  LDSection* sect_hdr = pInput.context()->getSection(pShndx);

  if (NULL == sect_hdr)
    unreachable(diag::unreachable_invalid_section_idx) << pShndx
                                                       << pInput.path().native();

  if (LDFileFormat::Ignore == sect_hdr->kind())
    return FragmentRef::Null();

  if (LDFileFormat::Group == sect_hdr->kind())
    return FragmentRef::Null();

  return FragmentRef::Create(*sect_hdr, pOffset);
}

/// getSymVisibility
ResolveInfo::Visibility
ELFReaderIF::getSymVisibility(uint8_t pVis) const
{
  return static_cast<ResolveInfo::Visibility>(pVis);
}

/// getSymValue - get the section offset of the symbol.
uint64_t ELFReaderIF::getSymValue(uint64_t pValue,
                                  uint16_t pShndx,
                                  const Input& pInput) const
{
  if (Input::Object == pInput.type()) {
    // In relocatable files, st_value holds alignment constraints for a symbol
    // whose section index is SHN_COMMON
    if (pShndx == llvm::ELF::SHN_COMMON || pShndx == llvm::ELF::SHN_ABS) {
      return pValue;
    }

    // In relocatable files, st_value holds a section offset for a defined symbol.
    // TODO:
    // if --just-symbols option are enabled, convert the value from section offset
    // to virtual address by adding input section's virtual address.
    // The section's virtual address in relocatable files is normally zero, but
    // people can use link script to change it.
    return pValue;
  }

  // In executable and shared object files, st_value holds a virtual address.
  // the virtual address is useless during linking.
  return 0x0;
}

//===----------------------------------------------------------------------===//
// ELFReader<32, true>
//===----------------------------------------------------------------------===//
/// readRegularSection - read a regular section and create fragments.
bool
ELFReader<32, true>::readRegularSection(Input& pInput, SectionData& pSD) const
{
  Fragment* frag = NULL;
  uint32_t offset = pInput.fileOffset() + pSD.getSection().offset();
  uint32_t size = pSD.getSection().size();

  MemoryRegion* region = pInput.memArea()->request(offset, size);
  if (NULL == region) {
    // If the input section's size is zero, we got a NULL region.
    // use a virtual fill fragment
    frag = new FillFragment(0x0, 0, 0);
  }
  else {
    frag = new RegionFragment(*region);
  }

  ObjectBuilder::AppendFragment(*frag, pSD);
  return true;
}

/// readSymbols - read ELF symbols and create LDSymbol
bool ELFReader<32, true>::readSymbols(Input& pInput,
                                      FragmentLinker& pLinker,
                                      const MemoryRegion& pRegion,
                                      const char* pStrTab) const
{
  // get number of symbols
  size_t entsize = pRegion.size()/sizeof(llvm::ELF::Elf32_Sym);
  const llvm::ELF::Elf32_Sym* symtab =
                 reinterpret_cast<const llvm::ELF::Elf32_Sym*>(pRegion.start());

  uint32_t st_name  = 0x0;
  uint32_t st_value = 0x0;
  uint32_t st_size  = 0x0;
  uint8_t  st_info  = 0x0;
  uint8_t  st_other = 0x0;
  uint16_t st_shndx = 0x0;

  // skip the first NULL symbol
  pInput.context()->addSymbol(LDSymbol::Null());

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

    // If the section should not be included, set the st_shndx SHN_UNDEF
    // - A section in interrelated groups are not included.
    if (pInput.type() == Input::Object &&
        st_shndx < llvm::ELF::SHN_LORESERVE &&
        st_shndx != llvm::ELF::SHN_UNDEF) {
      if (NULL == pInput.context()->getSection(st_shndx))
        st_shndx = llvm::ELF::SHN_UNDEF;
    }

    // get ld_type
    ResolveInfo::Type ld_type = getSymType(st_info, st_shndx);

    // get ld_desc
    ResolveInfo::Desc ld_desc = getSymDesc(st_shndx, pInput);

    // get ld_binding
    ResolveInfo::Binding ld_binding = getSymBinding((st_info >> 4), st_shndx, st_other);

    // get ld_value - ld_value must be section relative.
    uint64_t ld_value = getSymValue(st_value, st_shndx, pInput);

    // get the input fragment
    FragmentRef* ld_frag_ref = getSymFragmentRef(pInput,
                                                 st_shndx,
                                                 ld_value);

    // get ld_vis
    ResolveInfo::Visibility ld_vis = getSymVisibility(st_other);

    // get ld_name
    llvm::StringRef ld_name;
    if (ResolveInfo::Section == ld_type) {
      // Section symbol's st_name is the section index.
      LDSection* section = pInput.context()->getSection(st_shndx);
      assert(NULL != section && "get a invalid section");
      ld_name = llvm::StringRef(section->name());
    }
    else {
      ld_name = llvm::StringRef(pStrTab + st_name);
    }


    // push into FragmentLinker
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
      pInput.context()->addSymbol(input_sym);
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

//===----------------------------------------------------------------------===//
// ELFReader::read relocations - read ELF rela and rel, and create Relocation
//===----------------------------------------------------------------------===//
/// ELFReader::readRela - read ELF rela and create Relocation
bool ELFReader<32, true>::readRela(Input& pInput,
                                   FragmentLinker& pLinker,
                                   LDSection& pSection,
                                   const MemoryRegion& pRegion) const
{
  // get the number of rela
  size_t entsize = pRegion.size() / sizeof(llvm::ELF::Elf32_Rela);
  const llvm::ELF::Elf32_Rela* relaTab =
                reinterpret_cast<const llvm::ELF::Elf32_Rela*>(pRegion.start());

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
      fatal(diag::err_cannot_read_symbol) << r_sym << pInput.path();
    }

    pLinker.addRelocation(r_type, *symbol, pSection, r_offset, r_addend);
  } // end of for
  return true;
}

/// readRel - read ELF rel and create Relocation
bool ELFReader<32, true>::readRel(Input& pInput,
                                  FragmentLinker& pLinker,
                                  LDSection& pSection,
                                  const MemoryRegion& pRegion) const
{
  // get the number of rel
  size_t entsize = pRegion.size() / sizeof(llvm::ELF::Elf32_Rel);
  const llvm::ELF::Elf32_Rel* relTab =
                 reinterpret_cast<const llvm::ELF::Elf32_Rel*>(pRegion.start());

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
      fatal(diag::err_cannot_read_symbol) << r_sym << pInput.path();
    }

    pLinker.addRelocation(r_type, *symbol, pSection, r_offset);
  } // end of for
  return true;
}

