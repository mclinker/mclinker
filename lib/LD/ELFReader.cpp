//===- ELFReader.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/MC/MCLinker.h"
#include "mcld/LD/ELFReader.h"
#include "mcld/Support/MemoryArea.h"
#include "mcld/Support/MemoryRegion.h"

#include "mcld/Support/rslinker/utils/serialize.h"
#include "mcld/Support/rslinker/ELFObject.h"

#include <llvm/Support/ELF.h>
#include <llvm/ADT/Twine.h>
#include <sstream>

using namespace mcld;

//==========================
// ELFReader
Input::Type ELFReader::fileType(mcld::Input &pFile) const
{
  // TODO: We don't use this function for now
  llvm_unreachable("ELFReader::fileType");
  return Input::Object;
}

bool ELFReader::isLittleEndian(mcld::Input &pFile) const
{
  return true; // TODO
}
  
bool ELFReader::is64Bit(mcld::Input &pFile) const
{
  return false; // TODO
}

std::auto_ptr<ELFObject<32> >
ELFReader::createELFObject(mcld::Input &pFile) const
{
  MemoryArea* mapfile = pFile.memArea();
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
  return std::auto_ptr<ELFObject<32> >(ELFObject<32>::read(AR));
}

LDFileFormat::Kind
ELFReader::getLDSectionKind(const llvm::StringRef& name) const
{
  if (name.startswith(".text")) {
    return LDFileFormat::Text;
  }
  else if (name.startswith(".data")) {
    return LDFileFormat::Data;
  }
  else if (name.startswith(".bss")) {
    return LDFileFormat::BSS;
  }
  else if (name.startswith(".rodata")) {
    return LDFileFormat::ReadOnly;
  }
  else if (name.startswith(".debug")) {
    return LDFileFormat::Debug;
  }
  else {
    return LDFileFormat::MetaData;
  }
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

