/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <llvm/ADT/Triple.h>
#include <mcld/Target/TargetRegistry.h>
#include "ARM.h"
#include "ARMLDBackend.h"
#include "ARMELFObjectReader.h"
#include "ARMELFObjectWriter.h"

using namespace mcld;

ARMGNULDBackend::ARMGNULDBackend()
{
}

ARMGNULDBackend::~ARMGNULDBackend()
{
}

MCELFObjectTargetReader *ARMGNULDBackend::createObjectTargetReader() const
{
  return new ARMELFObjectReader();
}

MCELFObjectTargetWriter *ARMGNULDBackend::createObjectTargetWriter() const
{
  return new ARMELFObjectWriter();
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createARMLDBackend - the help funtion to create corresponding ARMLDBackend
///
TargetLDBackend* createARMLDBackend(const llvm::Target& pTarget,
                                    const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new ARMMachOLDBackend(createARMMachOArchiveReader,
                               createARMMachOObjectReader,
                               createARMMachOObjectWriter);
    **/
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new ARMCOFFLDBackend(createARMCOFFArchiveReader,
                               createARMCOFFObjectReader,
                               createARMCOFFObjectWriter);
    **/
  }
  return new ARMGNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeARMLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheARMTarget, createARMLDBackend);
}
