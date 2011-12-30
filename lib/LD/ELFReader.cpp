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
#include <mcld/Support/rslinker/ELFObject.h>
#include <mcld/LD/ELFReader.h>
#include <mcld/Target/GNULDBackend.h>
#include "mcld/Support/rslinker/utils/serialize.h"

#include <sstream>
#include <cstring>

using namespace mcld;

//==========================
// ELFReader
ELFReader::ELFReader(GNULDBackend& pBackend)
  : m_Backend(pBackend) {
}

bool ELFReader::isELF(Input& pInput) const
{
  // get the e_ident
  uint8_t* e_ident = pInput.memArea()->request(0,
                                             llvm::ELF::EI_NIDENT,
                                             false)->start();
  // the same endian
  if (llvm::sys::isLittleEndianHost() == isLittleEndian(pInput)) {
    if (0 == memcmp(llvm::ELF::ElfMagic, e_ident, 4))
      return true;
    return false;
  }
  // different endian
  else {
    if (llvm::ELF::ElfMagic[3] != e_ident[llvm::ELF::EI_MAG0] ||
        llvm::ELF::ElfMagic[2] != e_ident[llvm::ELF::EI_MAG1] ||
        llvm::ELF::ElfMagic[1] != e_ident[llvm::ELF::EI_MAG2] ||
        llvm::ELF::ElfMagic[0] != e_ident[llvm::ELF::EI_MAG3])
      return false;
    return true; 
  }
}

MCLDFile::Type ELFReader::fileType(Input &pInput) const
{
  // is it a ELF file?
  if (!isELF(pInput)) {
    return MCLDFile::Unknown;
  }

  // it is a ELF file
  // same endian
  uint8_t* data = pInput.memArea()->request(0,
                                           sizeof(llvm::ELF::Elf64_Ehdr),
                                           false)->start();

  uint32_t e_type = 0x0;
  // the same endian
  if (llvm::sys::isLittleEndianHost() == isLittleEndian(pInput)) {
    e_type |= data[llvm::ELF::EI_NIDENT];
    e_type |= (data[llvm::ELF::EI_NIDENT+1] << 8);
  }
  // different endian
  else {
    e_type |= (data[llvm::ELF::EI_NIDENT] << 8);
    e_type |= data[llvm::ELF::EI_NIDENT+1];
  }

  switch(e_type) {
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

bool ELFReader::isLittleEndian(Input &pInput) const
{
  // get the e_ident
  uint8_t* e_ident = pInput.memArea()->request(0,
                                               llvm::ELF::EI_NIDENT,
                                               false)->start();
  switch (e_ident[llvm::ELF::EI_DATA]) {
  case llvm::ELF::ELFDATANONE:
    llvm::report_fatal_error(llvm::Twine("invalied data encoding.\n"));
    break;
  case llvm::ELF::ELFDATA2LSB:
    return true;
  case llvm::ELF::ELFDATA2MSB:
    return false;
  }
  return false;
}
  
unsigned int ELFReader::bitclass(Input &pInput) const
{
  // get the e_ident
  uint8_t* e_ident = pInput.memArea()->request(0,
                                               llvm::ELF::EI_NIDENT,
                                               false)->start();
  switch (e_ident[llvm::ELF::EI_CLASS]) {
  case llvm::ELF::ELFCLASSNONE:
    llvm::report_fatal_error(llvm::Twine("invalied e_ident[EI_CLASS] class.\n"));
    break;
  case llvm::ELF::ELFCLASS32:
    return 32;
  case llvm::ELF::ELFCLASS64:
    return 64;
  }
  return 0;
}

ELFObject<32>* 
ELFReader::createELF32Object(Input &pInput) const
{
  MemoryArea* mapfile = pInput.memArea();
  size_t fsize = mapfile->size();
  unsigned char* image =
    mapfile->request (0, fsize, false /*iswrite*/)->start();

  if (fsize < EI_NIDENT) {
    llvm::report_fatal_error("ERROR: ELF identification corrupted.");
  }

  if (image[EI_CLASS] != ELFCLASS32 && image[EI_CLASS] != ELFCLASS64) {
    llvm::report_fatal_error("ERROR: Unknown machine class.");
  }

  // TODO: We can decide LE or BE, ELF-32 or ELF-64 from image
  // For now, ELF32-EL object is used
  ArchiveReaderLE AR(image, fsize);
  return ELFObject<32>::read(AR);
}

ELFObject<64>* 
ELFReader::createELF64Object(Input &pInput) const
{
  MemoryArea* mapfile = pInput.memArea();
  size_t fsize = mapfile->size();
  unsigned char* image =
    mapfile->request (0, fsize, false /*iswrite*/)->start();

  if (fsize < EI_NIDENT) {
    llvm::report_fatal_error("ERROR: ELF identification corrupted.");
  }

  if (image[EI_CLASS] != ELFCLASS32 && image[EI_CLASS] != ELFCLASS64) {
    llvm::report_fatal_error("ERROR: Unknown machine class.");
  }

  // TODO: We can decide LE or BE, ELF-32 or ELF-64 from image
  // For now, ELF32-EL object is used
  ArchiveReaderLE AR(image, fsize);
  return ELFObject<64>::read(AR);
}

LDFileFormat::Kind
ELFReader::getLDSectionKind(const ELFSectionHeader<32>& pHdr, const llvm::StringRef& pName) const
{
  // name rules
  if (pName.startswith(".debug"))
    return LDFileFormat::Debug;
  if (pName.startswith(".comment"))
    return LDFileFormat::MetaData;
  if (pName.startswith(".interp") || pName.startswith(".dynamic"))
    return LDFileFormat::Note;

  // type rules
  uint32_t type = pHdr.getType();
  switch(type) {
  case llvm::ELF::SHT_NULL:
    return LDFileFormat::Null;

  case llvm::ELF::SHT_SYMTAB:
  case llvm::ELF::SHT_DYNSYM:
  case llvm::ELF::SHT_STRTAB:
  case llvm::ELF::SHT_HASH:
    return LDFileFormat::NamePool;
  
  case llvm::ELF::SHT_PREINIT_ARRAY:
  case llvm::ELF::SHT_INIT_ARRAY:
  case llvm::ELF::SHT_FINI_ARRAY:
  case llvm::ELF::SHT_PROGBITS:
    return LDFileFormat::Regular;

  case llvm::ELF::SHT_RELA:
  case llvm::ELF::SHT_REL:
    return LDFileFormat::Relocation;

  case llvm::ELF::SHT_NOBITS:
    return LDFileFormat::BSS;

  case llvm::ELF::SHT_NOTE:
    return LDFileFormat::Note;

  case llvm::ELF::SHT_DYNAMIC:
    return LDFileFormat::Note;

  case llvm::ELF::SHT_SHLIB:
    return LDFileFormat::MetaData;

  default:
    if ( type >= llvm::ELF::SHT_LOPROC && type <= llvm::ELF::SHT_HIPROC) {
      return LDFileFormat::Target;
    }
    llvm::report_fatal_error(llvm::Twine("unsupported ELF section type: ") +
                             llvm::Twine(type) +
                             llvm::Twine(" of section ") +
                             pName +
                             llvm::Twine(".\n"));
  }
  return LDFileFormat::MetaData;
}

LDFileFormat::Kind
ELFReader::getLDSectionKind(const ELFSectionHeader<64>& pHdr,
                            const llvm::StringRef& pName) const
{
  // name rules
  if (llvm::StringRef::npos != pName.find(".debug"))
    return LDFileFormat::Debug;

  uint64_t type = pHdr.getType();
  switch(type) {
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
  case llvm::ELF::SHT_NOTE:
    return LDFileFormat::Note;
  case llvm::ELF::SHT_HASH:
  case llvm::ELF::SHT_DYNAMIC:
  case llvm::ELF::SHT_SHLIB:
    return LDFileFormat::MetaData;
  default:
    if ( type >= llvm::ELF::SHT_LOPROC && type <= llvm::ELF::SHT_HIPROC) {
      return LDFileFormat::Target;
    }
    llvm::report_fatal_error(llvm::Twine("unsupported ELF section type: ") +
                             llvm::Twine(type) + llvm::Twine(".\n"));
  }
  return LDFileFormat::MetaData;
}

ResolveInfo::Binding
ELFReader::getBindingResolveInfo(ELFSymbol<32>* sym, bool isDSO) const
{
  ResolveInfo::Binding ret;
  unsigned int bind = sym->getBindingAttribute();
  bool error = false;

  switch (bind) {
    case llvm::ELF::STB_LOCAL:
      ret = ResolveInfo::Local;
      error = isDSO;
      break;
    case llvm::ELF::STB_GLOBAL:
      ret = ResolveInfo::Global;
      break;
    case llvm::ELF::STB_WEAK:
      ret = ResolveInfo::Weak;
      break;
    default:
      error = (bind < llvm::ELF::STB_LOOS);
  }

  if (llvm::ELF::SHN_ABS == sym->getSectionIndex())
    ret = ResolveInfo::Absolute;

  if (error) {
    llvm::report_fatal_error(llvm::Twine("Unexcpect ELF symbol binding Type:") +
                             llvm::Twine(sym->getName(isDSO)) +
                             llvm::Twine(" with binding type `") +
                             llvm::Twine(bind) +
                             llvm::Twine(".\n"));
  }
  return ret;
}

ResolveInfo::Visibility
ELFReader::getVisibilityResolveInfo(ELFSymbol<32>* sym) const
{
  // Only Regular object check visibility
  ResolveInfo::Visibility ret;
  int visibility = sym->getVisibility();
  switch (visibility) {
    case 0:
      ret = ResolveInfo::Default;
      break;
    case 1:
      ret = ResolveInfo::Internal;
      break;
    case 2:
      ret = ResolveInfo::Hidden;
      break;
    case 3:
      ret = ResolveInfo::Protected;
      break;
    default:
      std::stringstream err_msg;
      err_msg << "Unexcpect Symbol Visibility Type:" << sym->getName(false);
      err_msg << " with visibility type " << visibility;
      llvm::report_fatal_error(err_msg.str());
  }
  return ret;
}

std::string ELFReader::getSymbolTypeName(unsigned int pType) const
{
  switch(pType) {
  case llvm::ELF::STT_NOTYPE:
    return "STT_NOTYPE";
  case llvm::ELF::STT_OBJECT:
    return "STT_OBJECT";
  case llvm::ELF::STT_FUNC:
    return "STT_FUNC";
  case llvm::ELF::STT_SECTION:
    return "STT_SECTION";
  case llvm::ELF::STT_FILE:
    return "STT_FILE";
  case llvm::ELF::STT_COMMON:
    return "STT_COMMON";
  case llvm::ELF::STT_TLS:
    return "STT_TLS";
  case llvm::ELF::STT_GNU_IFUNC:
    return "STT_GNU_IFUNC";
  default:
    if (pType >= llvm::ELF::STT_LOOS &&
        pType <= llvm::ELF::STT_HIOS)
      return "STT_LOOS .. STT_HIOS";
    if (pType >= llvm::ELF::STT_LOPROC &&
        pType <= llvm::ELF::STT_HIPROC)
      return "STT_LOPROC .. STT_HIPROC";
  }
  return "`unknow type in ELF'";
}

/// readELF32Rel
bool
ELFReader::readELF32Rel(const LDSection& pSection,
                        const MemoryRegion& pRegion,
                        MCLinker& pLinker)
{
  // TODO
  return true;
}

/// readELF32Rela
bool
ELFReader::readELF32Rela(const LDSection& pSection,
                         const MemoryRegion& pRegion,
                         MCLinker& pLinker)
{
  // TODO
  return true;
}

/// readELF64Rel
bool
ELFReader::readELF64Rel(const LDSection& pSection,
                        const MemoryRegion& pRegion,
                        MCLinker& pLinker)
{
  // TODO
  return true;
}

/// readELF64Rela
bool
ELFReader::readELF64Rela(const LDSection& pSection,
                         const MemoryRegion& pRegion,
                         MCLinker& pLinker)
{
  // TODO
  return true;
}

