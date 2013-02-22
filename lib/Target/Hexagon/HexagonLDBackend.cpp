//===- HexagonLDBackend.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Hexagon.h"
#include "HexagonELFDynamic.h"
#include "HexagonLDBackend.h"
#include "HexagonRelocator.h"
#include "HexagonGNUInfo.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/Casting.h>

#include <mcld/LinkerConfig.h>
#include <mcld/IRBuilder.h>
#include <mcld/Fragment/FillFragment.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Object/ObjectBuilder.h>

#include <cstring>

using namespace mcld;

//===----------------------------------------------------------------------===//
// HexagonLDBackend
//===----------------------------------------------------------------------===//
HexagonLDBackend::HexagonLDBackend(const LinkerConfig& pConfig, 
                                   HexagonGNUInfo* pInfo)
  : GNULDBackend(pConfig, pInfo),
    m_pRelocator(NULL),
    m_pGOT(NULL),
    m_pPLT(NULL),
    m_pRelDyn(NULL),
    m_pRelPLT(NULL),
    m_pDynamic(NULL),
    m_pGOTSymbol(NULL) {
}

HexagonLDBackend::~HexagonLDBackend()
{
  delete m_pRelocator;
  delete m_pGOT;
  delete m_pPLT;
  delete m_pRelDyn;
  delete m_pRelPLT;
  delete m_pDynamic;
}

bool HexagonLDBackend::initRelocator()
{
  if (NULL == m_pRelocator) {
    m_pRelocator = new HexagonRelocator(*this);
  }
  return true;
}

Relocator* HexagonLDBackend::getRelocator()
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

void HexagonLDBackend::doPreLayout(IRBuilder& pBuilder)
{
  // initialize .dynamic data
  if (!config().isCodeStatic() && NULL == m_pDynamic)
    m_pDynamic = new HexagonELFDynamic(*this, config());
}

void HexagonLDBackend::doPostLayout(Module& pModule, IRBuilder& pBuilder)
{
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
HexagonELFDynamic& HexagonLDBackend::dynamic()
{
  assert(NULL != m_pDynamic);
  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const HexagonELFDynamic& HexagonLDBackend::dynamic() const
{
  assert(NULL != m_pDynamic);
  return *m_pDynamic;
}

void HexagonLDBackend::scanRelocation(Relocation& pReloc,
                                      IRBuilder& pBuilder,
                                      Module& pModule,
                                      LDSection& pSection)
{
  pReloc.updateAddend();
}

uint64_t HexagonLDBackend::emitSectionData(const LDSection& pSection,
                                          MemoryRegion& pRegion) const
{
  return 0;
}

HexagonGOT& HexagonLDBackend::getGOT()
{
  assert(NULL != m_pGOT);
  return *m_pGOT;
}

const HexagonGOT& HexagonLDBackend::getGOT() const
{
  assert(NULL != m_pGOT);
  return *m_pGOT;
}

HexagonPLT& HexagonLDBackend::getPLT()
{
  assert(NULL != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

const HexagonPLT& HexagonLDBackend::getPLT() const
{
  assert(NULL != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

OutputRelocSection& HexagonLDBackend::getRelDyn()
{
  assert(NULL != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

const OutputRelocSection& HexagonLDBackend::getRelDyn() const
{
  assert(NULL != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

OutputRelocSection& HexagonLDBackend::getRelPLT()
{
  assert(NULL != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

const OutputRelocSection& HexagonLDBackend::getRelPLT() const
{
  assert(NULL != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

unsigned int
HexagonLDBackend::getTargetSectionOrder(const LDSection& pSectHdr) const
{
  const ELFFileFormat* file_format = getOutputFormat();

  if (&pSectHdr == &file_format->getGOT()) {
    if (config().options().hasNow())
      return SHO_RELRO;
    return SHO_RELRO_LAST;
  }

  if (&pSectHdr == &file_format->getPLT())
    return SHO_PLT;

  return SHO_UNDEFINED;
}

void HexagonLDBackend::initTargetSections(Module& pModule, 
                                          ObjectBuilder& pBuilder)
{
  if (LinkerConfig::Object != config().codeGenType()) {
    ELFFileFormat* file_format = getOutputFormat();
    // initialize .got
    LDSection& got = file_format->getGOT();
    m_pGOT = new HexagonGOT(got);

    // initialize .plt
    LDSection& plt = file_format->getPLT();
    m_pPLT = new HexagonPLT(plt,
                        *m_pGOT,
                        config());

    // initialize .rel.plt
    LDSection& relplt = file_format->getRelPlt();
    relplt.setLink(&plt);
    m_pRelPLT = new OutputRelocSection(pModule, relplt);

    // initialize .rel.dyn
    LDSection& reldyn = file_format->getRelDyn();
    m_pRelDyn = new OutputRelocSection(pModule, reldyn);

  }
}

void HexagonLDBackend::initTargetSymbols(IRBuilder& pBuilder, Module& pModule)
{
  if (LinkerConfig::Object != config().codeGenType()) {
    // Define the symbol _GLOBAL_OFFSET_TABLE_ if there is a symbol with the
    // same name in input
    m_pGOTSymbol = pBuilder.AddSymbol<IRBuilder::AsReferred, IRBuilder::Resolve>(
                                                    "_GLOBAL_OFFSET_TABLE_",
                                                    ResolveInfo::Object,
                                                    ResolveInfo::Define,
                                                    ResolveInfo::Local,
                                                    0x0,  // size
                                                    0x0,  // value
                                                    FragmentRef::Null(),
                                                    ResolveInfo::Hidden);
  }
}

/// finalizeSymbol - finalize the symbol value
bool HexagonLDBackend::finalizeTargetSymbols()
{
  return true;
}

/// doCreateProgramHdrs - backend can implement this function to create the
/// target-dependent segments
void HexagonLDBackend::doCreateProgramHdrs(Module& pModule)
{
  // TODO
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createHexagonLDBackend - the help funtion to create corresponding 
/// HexagonLDBackend
TargetLDBackend* createHexagonLDBackend(const llvm::Target& pTarget,
                                    const LinkerConfig& pConfig)
{
  if (pConfig.targets().triple().isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new HexagonMachOLDBackend(createHexagonMachOArchiveReader,
                               createHexagonMachOObjectReader,
                               createHexagonMachOObjectWriter);
    **/
  }
  if (pConfig.targets().triple().isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new HexagonCOFFLDBackend(createHexagonCOFFArchiveReader,
                               createHexagonCOFFObjectReader,
                               createHexagonCOFFObjectWriter);
    **/
  }
  return new HexagonLDBackend(pConfig, 
                              new HexagonGNUInfo(pConfig.targets().triple()));
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// Force static initialization.
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeHexagonLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheHexagonTarget, 
                                                createHexagonLDBackend);
}
