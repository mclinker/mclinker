//===- AArch64LDBackend.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AArch64.h"
#include "AArch64GNUInfo.h"
#include "AArch64LDBackend.h"
#include "AArch64Relocator.h"

#include <cstring>

#include <llvm/ADT/Triple.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Casting.h>

#include <mcld/IRBuilder.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Fragment/FillFragment.h>
#include <mcld/Fragment/AlignFragment.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/Fragment/Stub.h>
#include <mcld/Fragment/NullFragment.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/StubFactory.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/ELFFileFormat.h>
#include <mcld/LD/ELFSegmentFactory.h>
#include <mcld/LD/ELFSegment.h>
#include <mcld/Target/ELFAttribute.h>
#include <mcld/Target/GNUInfo.h>
#include <mcld/Object/ObjectBuilder.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// AArch64GNULDBackend
//===----------------------------------------------------------------------===//
AArch64GNULDBackend::AArch64GNULDBackend(const LinkerConfig& pConfig,
                                         GNUInfo* pInfo)
  : GNULDBackend(pConfig, pInfo),
    m_pRelocator(NULL),
    m_pGOT(NULL),
    m_pPLT(NULL),
    m_pRelaDyn(NULL),
    m_pRelaPLT(NULL),
    // m_pAttrData(NULL),
    m_pDynamic(NULL),
    m_pGOTSymbol(NULL),
    m_pAttributes(NULL) {
}

AArch64GNULDBackend::~AArch64GNULDBackend()
{
  if (m_pRelocator != NULL)
    delete m_pRelocator;
}

void AArch64GNULDBackend::initTargetSections(Module& pModule,
                                             ObjectBuilder& pBuilder)
{
  // TODO

  if (LinkerConfig::Object != config().codeGenType()) {
    ELFFileFormat* file_format = getOutputFormat();

    // initialize .got
    LDSection& got = file_format->getGOT();
    m_pGOT = new AArch64GOT(got);

    // initialize .plt
    LDSection& plt = file_format->getPLT();
    m_pPLT = new AArch64PLT(plt, *m_pGOT);

    // initialize .rela.plt
    LDSection& relaplt = file_format->getRelaPlt();
    relaplt.setLink(&plt);
    m_pRelaPLT = new OutputRelocSection(pModule, relaplt);

    // initialize .rela.dyn
    LDSection& reladyn = file_format->getRelaDyn();
    m_pRelaDyn = new OutputRelocSection(pModule, reladyn);
  }
}

void AArch64GNULDBackend::initTargetSymbols(IRBuilder& pBuilder,
                                            Module& pModule)
{
  // TODO
}

bool AArch64GNULDBackend::initRelocator()
{
  if (NULL == m_pRelocator) {
    m_pRelocator = new AArch64Relocator(*this, config());
  }
  return true;
}

Relocator* AArch64GNULDBackend::getRelocator()
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

void AArch64GNULDBackend::doPreLayout(IRBuilder& pBuilder)
{
  // TODO
}

void AArch64GNULDBackend::doPostLayout(Module& pModule, IRBuilder& pBuilder)
{
  // TODO
}

AArch64ELFDynamic& AArch64GNULDBackend::dynamic()
{
  assert(NULL != m_pDynamic);
  return *m_pDynamic;
}

const AArch64ELFDynamic& AArch64GNULDBackend::dynamic() const
{
  assert(NULL != m_pDynamic);
  return *m_pDynamic;
}

uint64_t AArch64GNULDBackend::emitSectionData(const LDSection& pSection,
                                              MemoryRegion& pRegion) const
{
  // TODO
  return pRegion.size();
}

unsigned int
AArch64GNULDBackend::getTargetSectionOrder(const LDSection& pSectHdr) const
{
  // TODO
  return SHO_UNDEFINED;
}

bool AArch64GNULDBackend::doRelax(Module& pModule,
                                  IRBuilder& pBuilder,
                                  bool& pFinished)
{
  // TODO
  return false;
}

bool AArch64GNULDBackend::initTargetStubs()
{
  // TODO
  return true;
}

void AArch64GNULDBackend::doCreateProgramHdrs(Module& pModule)
{
  // TODO
}

bool AArch64GNULDBackend::finalizeTargetSymbols()
{
  // TODO
  return true;
}

bool AArch64GNULDBackend::mergeSection(Module& pModule,
                                       const Input& pInput,
                                       LDSection& pSection)
{
  // TODO
  return true;
}

bool AArch64GNULDBackend::readSection(Input& pInput, SectionData& pSD)
{
  // TODO
  return true;
}

AArch64GOT& AArch64GNULDBackend::getGOT()
{
  assert(NULL != m_pGOT && "GOT section not exist");
  return *m_pGOT;
}

const AArch64GOT& AArch64GNULDBackend::getGOT() const
{
  assert(NULL != m_pGOT && "GOT section not exist");
  return *m_pGOT;
}

AArch64PLT& AArch64GNULDBackend::getPLT()
{
  assert(NULL != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

const AArch64PLT& AArch64GNULDBackend::getPLT() const
{
  assert(NULL != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

OutputRelocSection& AArch64GNULDBackend::getRelaDyn()
{
  assert(NULL != m_pRelaDyn && ".rela.dyn section not exist");
  return *m_pRelaDyn;
}

const OutputRelocSection& AArch64GNULDBackend::getRelaDyn() const
{
  assert(NULL != m_pRelaDyn && ".rela.dyn section not exist");
  return *m_pRelaDyn;
}

OutputRelocSection& AArch64GNULDBackend::getRelaPLT()
{
  assert(NULL != m_pRelaPLT && ".rela.plt section not exist");
  return *m_pRelaPLT;
}

const OutputRelocSection& AArch64GNULDBackend::getRelaPLT() const
{
  assert(NULL != m_pRelaPLT && ".rela.plt section not exist");
  return *m_pRelaPLT;
}

namespace mcld {

//===----------------------------------------------------------------------===//
//  createAArch64LDBackend - the help funtion to create corresponding
//  AArch64LDBackend
//===----------------------------------------------------------------------===//
TargetLDBackend* createAArch64LDBackend(const LinkerConfig& pConfig)
{
  if (pConfig.targets().triple().isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new AArch64MachOLDBackend(createAArch64MachOArchiveReader,
                                     createAArch64MachOObjectReader,
                                     createAArch64MachOObjectWriter);
    **/
  }
  if (pConfig.targets().triple().isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new AArch64COFFLDBackend(createAArch64COFFArchiveReader,
                                    createAArch64COFFObjectReader,
                                    createAArch64COFFObjectWriter);
    **/
  }
  return new AArch64GNULDBackend(pConfig,
     new AArch64GNUInfo(pConfig.targets().triple()));
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// Force static initialization.
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeAArch64LDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheAArch64Target,
                                                createAArch64LDBackend);
}

