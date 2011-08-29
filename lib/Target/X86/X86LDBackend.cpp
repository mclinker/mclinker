/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <llvm/ADT/Triple.h>
#include <mcld/Target/TargetRegistry.h>
#include "X86.h"
#include "X86LDBackend.h"
#include "X86ELFObjectReader.h"
#include "X86ELFObjectWriter.h"

using namespace mcld;

X86GNULDBackend::X86GNULDBackend()
{
}

X86GNULDBackend::~X86GNULDBackend()
{
}

MCELFObjectTargetReader *X86GNULDBackend::createObjectTargetReader() const
{
  return new X86ELFObjectReader();
}

MCELFObjectTargetWriter *X86GNULDBackend::createObjectTargetWriter() const
{
  return new X86ELFObjectWriter();
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createX86LDBackend - the help funtion to create corresponding X86LDBackend
///
TargetLDBackend* createX86LDBackend(const llvm::Target& pTarget,
                                    const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new X86MachOLDBackend(createX86MachOArchiveReader,
                               createX86MachOObjectReader,
                               createX86MachOObjectWriter);
    **/
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new X86COFFLDBackend(createX86COFFArchiveReader,
                               createX86COFFObjectReader,
                               createX86COFFObjectWriter);
    **/
  }
  return new X86GNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeX86LDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheX86Target, createX86LDBackend);
}
