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
  size_t fsize = pFile.filesize();
  unsigned char* image =
    pFile.memArea()->request ( 0, fsize, false /*iswrite*/)->start();

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

ResolveInfo::Binding
ELFReader::getBindingResolveInfo(ELFSymbol<32>* sym, bool isDSO) const
{
  ResolveInfo::Binding ret;
  int bind = sym->getBindingAttribute();
  bool error = false;

  switch (bind) {
    case 0:
      ret = ResolveInfo::Local;
      error = isDSO;
      break;
    case 1:
      ret = ResolveInfo::Global;
      break;
    case 2:
      ret = ResolveInfo::Weak;
      break;
    default:
      error = true;
  }

  if (error) {
    std::stringstream err_msg;
    err_msg << "Unexcpect Symbol Binding Type:" << sym->getName(isDSO);
    err_msg << " with binding type " << bind;
    llvm::report_fatal_error(err_msg.str());
  }

  //FIXME: Check ABS Symbol
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

