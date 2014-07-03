//===- AArch64LDBackend.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AArch64.h"
#include "AArch64ELFDynamic.h"
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
    m_pGOTPLT(NULL),
    m_pPLT(NULL),
    m_pRelaDyn(NULL),
    m_pRelaPLT(NULL),
    // m_pAttrData(NULL),
    m_pDynamic(NULL),
    m_pGOTSymbol(NULL)
    // m_pAttributes(NULL)
{
}

AArch64GNULDBackend::~AArch64GNULDBackend()
{
  if (m_pRelocator != NULL)
    delete m_pRelocator;
  if (m_pGOT == m_pGOTPLT) {
    if (m_pGOT != NULL)
      delete m_pGOT;
  } else {
    if (m_pGOT != NULL)
      delete m_pGOT;
    if (m_pGOTPLT != NULL)
      delete m_pGOTPLT;
  }
  if (m_pPLT != NULL)
    delete m_pPLT;
  if (m_pRelaDyn != NULL)
    delete m_pRelaDyn;
  if (m_pRelaPLT != NULL)
    delete m_pRelaPLT;
  if (m_pDynamic != NULL)
    delete m_pDynamic;
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
    if (config().options().hasNow()) {
      // when -z now is given, there will be only one .got section (contains
      // both GOTPLT and normal GOT entries), create GOT0 for .got section and
      // set m_pGOTPLT to the same .got
      m_pGOT->createGOT0();
      m_pGOTPLT = m_pGOT;
    }
    else {
      // Otherwise, got should be seperated to two sections, .got and .got.plt
      // initialize .got.plt
      LDSection& gotplt = file_format->getGOTPLT();
      m_pGOTPLT = new AArch64GOT(gotplt);
      m_pGOTPLT->createGOT0();
    }

    // initialize .plt
    LDSection& plt = file_format->getPLT();
    m_pPLT = new AArch64PLT(plt, *m_pGOTPLT);

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
  // Define the symbol _GLOBAL_OFFSET_TABLE_ if there is a symbol with the
  // same name in input
  if (LinkerConfig::Object != config().codeGenType()) {
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
  // TODO
}

bool AArch64GNULDBackend::initRelocator()
{
  if (NULL == m_pRelocator) {
    m_pRelocator = new AArch64Relocator(*this, config());
  }
  return true;
}

const Relocator* AArch64GNULDBackend::getRelocator() const
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

Relocator* AArch64GNULDBackend::getRelocator()
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

void AArch64GNULDBackend::defineGOTSymbol(IRBuilder& pBuilder)
{
  // define symbol _GLOBAL_OFFSET_TABLE_ when .got create
  if (m_pGOTSymbol != NULL) {
    pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Unresolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pGOTPLT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
  else {
    m_pGOTSymbol = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pGOTPLT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
}

void AArch64GNULDBackend::doPreLayout(IRBuilder& pBuilder)
{
  // initialize .dynamic data
  if (!config().isCodeStatic() && NULL == m_pDynamic)
    m_pDynamic = new AArch64ELFDynamic(*this, config());

  if (LinkerConfig::Object != config().codeGenType()) {
    // set .got size
    if (config().options().hasNow()) {
      // when building shared object, the GOTPLT section is must
      if (LinkerConfig::DynObj == config().codeGenType() ||
          m_pGOT->hasGOT1() ||
          NULL != m_pGOTSymbol) {
        m_pGOT->finalizeSectionSize();
        defineGOTSymbol(pBuilder);
      }
    }
    else {
      // when building shared object, the GOTPLT section is must
      if (LinkerConfig::DynObj == config().codeGenType() ||
          m_pGOTPLT->hasGOT1() ||
          NULL != m_pGOTSymbol) {
        m_pGOTPLT->finalizeSectionSize();
        defineGOTSymbol(pBuilder);
      }
      if (m_pGOT->hasGOT1())
        m_pGOT->finalizeSectionSize();
    }

    // set .plt size
    if (m_pPLT->hasPLT1())
      m_pPLT->finalizeSectionSize();

    ELFFileFormat* file_format = getOutputFormat();
    // set .rela.dyn size
    if (!m_pRelaDyn->empty()) {
      assert(!config().isCodeStatic() &&
            "static linkage should not result in a dynamic relocation section");
      file_format->getRelaDyn().setSize(
                                m_pRelaDyn->numOfRelocs() * getRelaEntrySize());
    }

    // set .rela.plt size
    if (!m_pRelaPLT->empty()) {
      assert(!config().isCodeStatic() &&
            "static linkage should not result in a dynamic relocation section");
      file_format->getRelaPlt().setSize(
                                m_pRelaPLT->numOfRelocs() * getRelaEntrySize());
    }
  }
}

void AArch64GNULDBackend::doPostLayout(Module& pModule, IRBuilder& pBuilder)
{
  const ELFFileFormat *file_format = getOutputFormat();

  // apply PLT
  if (file_format->hasPLT()) {
    assert(NULL != m_pPLT);
    m_pPLT->applyPLT0();
    m_pPLT->applyPLT1();
  }

  // apply GOTPLT
  if ((config().options().hasNow() && file_format->hasGOT()) ||
       file_format->hasGOTPLT()) {
    assert(NULL != m_pGOTPLT);
    if (LinkerConfig::DynObj == config().codeGenType())
      m_pGOTPLT->applyGOT0(file_format->getDynamic().addr());
    else {
      // executable file and object file? should fill with zero.
      m_pGOTPLT->applyGOT0(0);
    }
  }
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
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const ELFFileFormat* file_format = getOutputFormat();

  if (file_format->hasPLT() && (&pSection == &(file_format->getPLT()))) {
    uint64_t result = m_pPLT->emit(pRegion);
    return result;
  }

  if (file_format->hasGOT() && (&pSection == &(file_format->getGOT()))) {
    uint64_t result = m_pGOT->emit(pRegion);
    return result;
  }

  if (file_format->hasGOTPLT() && (&pSection == &(file_format->getGOTPLT()))) {
    uint64_t result = m_pGOT->emit(pRegion);
    return result;
  }

  // TODO
  return pRegion.size();
}

unsigned int
AArch64GNULDBackend::getTargetSectionOrder(const LDSection& pSectHdr) const
{
  const ELFFileFormat* file_format = getOutputFormat();

  if (file_format->hasGOT() && (&pSectHdr == &file_format->getGOT())) {
    if (config().options().hasNow())
      return SHO_RELRO;
    return SHO_RELRO_LAST;
  }

  if (file_format->hasGOTPLT() && (&pSectHdr == &file_format->getGOTPLT()))
    return SHO_NON_RELRO_FIRST;

  if (file_format->hasPLT() && (&pSectHdr == &file_format->getPLT()))
    return SHO_PLT;

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

AArch64GOT& AArch64GNULDBackend::getGOTPLT()
{
  assert(NULL != m_pGOTPLT && "GOTPLT section not exist");
  return *m_pGOTPLT;
}

const AArch64GOT& AArch64GNULDBackend::getGOTPLT() const
{
  assert(NULL != m_pGOTPLT && "GOTPLT section not exist");
  return *m_pGOTPLT;
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

