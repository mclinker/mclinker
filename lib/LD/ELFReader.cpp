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
  default:
    if (pType >= llvm::ELF::SHT_LOPROC && pType <= llvm::ELF::SHT_HIPROC)
      return LDFileFormat::Target;
    llvm::report_fatal_error(llvm::Twine("unsupported ELF section type: ") +
                             llvm::Twine(pType) + llvm::Twine(".\n"));
  }
  return LDFileFormat::MetaData;
}

/// getSymDesc
ResolveInfo::Desc ELFReaderIF::getSymDesc(uint16_t pShndx) const
{
  if (pShndx == llvm::ELF::SHN_UNDEF)
    return ResolveInfo::Undefined;

  if (pShndx < llvm::ELF::SHN_LORESERVE)
    return ResolveInfo::Define;

  if (pShndx == llvm::ELF::SHN_ABS)
    return ResolveInfo::Define;

  if (pShndx == llvm::ELF::SHN_COMMON)
    return ResolveInfo::Common;

  // FIXME: ELF weak alias should be ResolveInfo::Indirect
  return ResolveInfo::NoneDesc;
}

/// getSymBinding
ResolveInfo::Binding
ELFReaderIF::getSymBinding(uint8_t pBinding, uint16_t pShndx) const
{
  if (pShndx == llvm::ELF::SHN_ABS)
    return ResolveInfo::Absolute;

  switch(pBinding) {
  case llvm::ELF::STB_LOCAL:
    return ResolveInfo::Local;
  case llvm::ELF::STB_GLOBAL:
    return ResolveInfo::Global;
  case llvm::ELF::STB_WEAK:
    return ResolveInfo::Weak;
  }
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

