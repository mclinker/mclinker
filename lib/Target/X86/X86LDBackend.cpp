//===- X86LDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "X86.h"
#include "X86ELFDynamic.h"
#include "X86LDBackend.h"
#include "X86RelocationFactory.h"

#include <llvm/ADT/Triple.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/LD/LDSection.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/SectionMap.h>

using namespace mcld;

X86GNULDBackend::X86GNULDBackend()
  : m_pRelocFactory(0), m_pGOT(0), m_pDynamic(0)
{
}

X86GNULDBackend::~X86GNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
  if (0 != m_pGOT)
    delete m_pGOT;
  if (0 != m_pDynamic)
    delete m_pDynamic;
}

RelocationFactory* X86GNULDBackend::getRelocFactory()
{
  assert(NULL != m_pRelocFactory);
  return m_pRelocFactory;
}

bool X86GNULDBackend::initRelocFactory(const MCLinker& pLinker)
{
  if (NULL == m_pRelocFactory) {
    m_pRelocFactory = new X86RelocationFactory(1024, *this);
    m_pRelocFactory->setLayout(pLinker.getLayout());
  }
  return true;
}

void X86GNULDBackend::doPreLayout(const Output& pOutput,
                                  const MCLDInfo& pInfo,
                                  MCLinker& pLinker)
{
  // add any needed modification before layout
}

void X86GNULDBackend::doPostLayout(const Output& pOutput,
                                   const MCLDInfo& pInfo,
                                   MCLinker& pLinker)
{
  // emit program headers
  if(pOutput.type() == Output::DynObj || pOutput.type() == Output::Exec)
    emitProgramHdrs(pOutput);
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
X86ELFDynamic& X86GNULDBackend::dynamic()
{
  if (NULL == m_pDynamic)
    m_pDynamic = new X86ELFDynamic(*this);

  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const X86ELFDynamic& X86GNULDBackend::dynamic() const
{
  assert( NULL != m_pDynamic);
  return *m_pDynamic;
}

uint32_t X86GNULDBackend::machine() const
{
  return ELF::EM_386;
}

X86GOT& X86GNULDBackend::getGOT()
{
  assert(0 != m_pGOT);
  return *m_pGOT;
}

const X86GOT& X86GNULDBackend::getGOT() const
{
  assert(0 != m_pGOT);
  return *m_pGOT;
}

unsigned int X86GNULDBackend::bitclass() const
{
  return 32;
}

bool X86GNULDBackend::initTargetSectionMap(SectionMap& pSectionMap)
{
  return true;
}

void X86GNULDBackend::initTargetSections(MCLinker& pLinker)
{
  LDSection& got = pLinker.getOrCreateOutputSectHdr(".got",
                                                    LDFileFormat::Target,
                                                    ELF::SHT_PROGBITS,
                                                    ELF::SHF_ALLOC | ELF::SHF_WRITE);
  llvm::MCSectionData& GOTSectionData = pLinker.getOrCreateSectData(got);

  m_pGOT = new X86GOT(got, GOTSectionData);
}

void X86GNULDBackend::scanRelocation(Relocation& pReloc,
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     unsigned int pType)
{
  llvm::report_fatal_error(llvm::Twine("Unexpected reloc ") +
			   llvm::Twine(pReloc.type()) +
			   llvm::Twine("in object file"));
}

/// finalizeSymbol - finalize the symbol value
/// If the symbol's reserved field is not zero, MCLinker will call back this
/// function to ask the final value of the symbol
bool X86GNULDBackend::finalizeSymbol(LDSymbol& pSymbol) const
{
  return false;
  // FIXME
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
