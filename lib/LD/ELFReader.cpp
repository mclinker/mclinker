//===- ELFReader.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/ELF.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/Host.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/LD/ELFReader.h>
#include <mcld/Target/GNULDBackend.h>
#include <cstring>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFReaderIF
/// getLDSectionKind
LDFileFormat::Kind
ELFReaderIF::getLDSectionKind(uint32_t pType, const char* pName) const
{
  // name rules
  llvm::StringRef name(pName);
  if (llvm::StringRef::npos != name.find(".debug"))
    return LDFileFormat::Debug;
  if (name.startswith(".comment"))
    return LDFileFormat::MetaData;
  if (name.startswith(".interp") || name.startswith(".dynamic"))
    return LDFileFormat::Note;
  if (name.startswith(".eh_frame") ||
      name.startswith(".eh_frame_hdr") ||
      name.startswith(".gcc_except_table"))
    return LDFileFormat::Exception;

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
    return LDFileFormat::NamePool;
  case llvm::ELF::SHT_RELA:
  case llvm::ELF::SHT_REL:
    return LDFileFormat::Relocation;
  case llvm::ELF::SHT_NOBITS:
    return LDFileFormat::BSS;
  case llvm::ELF::SHT_DYNAMIC:
  case llvm::ELF::SHT_NOTE:
    return LDFileFormat::Note;
  case llvm::ELF::SHT_HASH:
  case llvm::ELF::SHT_SHLIB:
    return LDFileFormat::MetaData;
  case llvm::ELF::SHT_GROUP:
    return LDFileFormat::Group;
  case llvm::ELF::SHT_GNU_versym:
  case llvm::ELF::SHT_GNU_verdef:
  case llvm::ELF::SHT_GNU_verneed:
    return LDFileFormat::Version;
  default:
    if ((pType >= llvm::ELF::SHT_LOPROC && pType <= llvm::ELF::SHT_HIPROC) ||
        (pType >= llvm::ELF::SHT_LOOS && pType <= llvm::ELF::SHT_HIOS) ||
        (pType >= llvm::ELF::SHT_LOUSER && pType <= llvm::ELF::SHT_HIUSER))
      return LDFileFormat::Target;
    llvm::report_fatal_error(llvm::Twine("unsupported ELF section type: ") +
                             llvm::Twine(pType) + llvm::Twine(".\n"));
  }
  return LDFileFormat::MetaData;
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
    if (NULL == pInput.context()->getSection(pShndx))
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
MCFragmentRef*
ELFReaderIF::getSymFragmentRef(Input& pInput,
                               MCLinker& pLinker,
                               uint16_t pShndx,
                               uint32_t pOffset) const
{
  
  if (pShndx == llvm::ELF::SHN_UNDEF || pShndx >= llvm::ELF::SHN_LORESERVE)
    return NULL;

  LDSection* sect_hdr = pInput.context()->getSection(pShndx);

  if (NULL == sect_hdr) {
    llvm::report_fatal_error(llvm::Twine("section[") +
                             llvm::Twine(pShndx) +
                             llvm::Twine("] is invalid in file `") +
                             pInput.path().native() +
                             llvm::Twine("'.\n"));
  }
  
  MCFragmentRef* result = pLinker.getLayout().getFragmentRef(*sect_hdr, pOffset);
  return result;
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

