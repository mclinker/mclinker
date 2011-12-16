//===- MipsLDBackend.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Triple.h>
#include "mcld/Support/TargetRegistry.h"
#include "Mips.h"
#include "MipsLDBackend.h"
#include "MipsRelocationFactory.h"

using namespace mcld;

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(0), m_GOT() {
}

MipsGNULDBackend::~MipsGNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
}

RelocationFactory* MipsGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new MipsRelocationFactory(1024, *this);
  return m_pRelocFactory;
}

uint32_t MipsGNULDBackend::machine() const
{
  return EM_MIPS;
}

bool MipsGNULDBackend::isLittleEndian() const
{
  /** is Mips little endian? I guess it is.**/
  return true;
}

unsigned int MipsGNULDBackend::bitclass() const
{
  return 32;
}

MipsGOT& MipsGNULDBackend::getGOT()
{
  return m_GOT;
}

const MipsGOT& MipsGNULDBackend::getGOT() const
{
  return m_GOT;
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createMipsLDBackend - the help funtion to create corresponding MipsLDBackend
///
static TargetLDBackend* createMipsLDBackend(const llvm::Target& pTarget,
                                            const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
  }
  return new MipsGNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeMipsLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheMipselTarget,
                                                createMipsLDBackend);
}
