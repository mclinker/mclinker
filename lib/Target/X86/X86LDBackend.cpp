//===- X86LDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/TargetRegistry.h"
#include "llvm/ADT/Triple.h"
#include "X86.h"
#include "X86LDBackend.h"
#include "X86ELFObjectReader.h"

namespace mcld {

X86GNULDBackend::X86GNULDBackend()
  : m_pRelocFactory(0) {
}

X86GNULDBackend::~X86GNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
}

MCELFObjectTargetReader *X86GNULDBackend::createObjectTargetReader() const
{
  return new X86ELFObjectReader();
}

MCELFObjectTargetWriter *X86GNULDBackend::createObjectTargetWriter() const
{
  return new X86ELFObjectWriter();
}

X86RelocationFactory* X86GNULDBackend::getRelocFactory()
{
  if (0 == m_pRelocFactory)
    m_pRelocFactory = new X86RelocationFactory(1024);
  return m_pRelocFactory;
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

//=============================
// Force static initialization.
extern "C" void LLVMInitializeX86LDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheX86Target, createX86LDBackend);
}

} // namespace of mcld
