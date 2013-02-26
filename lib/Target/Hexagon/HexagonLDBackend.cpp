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
#include "HexagonAbsoluteStub.h"

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
#include <mcld/Fragment/Stub.h>
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/StubFactory.h>
#include <mcld/LD/LDContext.h>


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

bool HexagonLDBackend::initTargetStubs()
{
  if (NULL != getStubFactory()) {
    getStubFactory()->addPrototype
                        (new HexagonAbsoluteStub(config().isCodeIndep()));
    return true;
  }
  return false;
}

bool HexagonLDBackend::initBRIslandFactory()
{
  if (NULL == m_pBRIslandFactory) {
    m_pBRIslandFactory = new BranchIslandFactory(maxBranchOffset(), 0);
  }
  return true;
}

bool HexagonLDBackend::initStubFactory()
{
  if (NULL == m_pStubFactory) {
    m_pStubFactory = new StubFactory();
  }
  return true;
}

bool HexagonLDBackend::doRelax(Module& pModule, IRBuilder& pBuilder,
                               bool& pFinished)
{
  assert(NULL != getStubFactory() && NULL != getBRIslandFactory());
  bool isRelaxed = false;
  ELFFileFormat* file_format = getOutputFormat();
  // check branch relocs and create the related stubs if needed
  Module::obj_iterator input, inEnd = pModule.obj_end();
  for (input = pModule.obj_begin(); input != inEnd; ++input) {
    LDContext::sect_iterator rs, rsEnd = (*input)->context()->relocSectEnd();
    for (rs = (*input)->context()->relocSectBegin(); rs != rsEnd; ++rs) {
      if (LDFileFormat::Ignore == (*rs)->kind() || !(*rs)->hasRelocData())
        continue;
      RelocData::iterator reloc, rEnd = (*rs)->getRelocData()->end();
      for (reloc = (*rs)->getRelocData()->begin(); reloc != rEnd; ++reloc) {
        switch (reloc->type()) {
          case llvm::ELF::R_HEX_B22_PCREL:
          case llvm::ELF::R_HEX_B15_PCREL:
          case llvm::ELF::R_HEX_B7_PCREL:
          case llvm::ELF::R_HEX_B13_PCREL:
          case llvm::ELF::R_HEX_B9_PCREL: {
            Relocation* relocation = llvm::cast<Relocation>(reloc);
            uint64_t sym_value = 0x0;
            LDSymbol* symbol = relocation->symInfo()->outSymbol();
            if (symbol->hasFragRef()) {
              uint64_t value = symbol->fragRef()->getOutputOffset();
              uint64_t addr =
                symbol->fragRef()->frag()->getParent()->getSection().addr();
              sym_value = addr + value;
            }
            Stub* stub = getStubFactory()->create(*relocation, // relocation
                                                  sym_value, //symbol value
                                                  pBuilder,
                                                  *getBRIslandFactory());
            if (NULL != stub) {
              assert(NULL != stub->symInfo());
              // increase the size of .symtab and .strtab
              LDSection& symtab = file_format->getSymTab();
              LDSection& strtab = file_format->getStrTab();
              symtab.setSize(symtab.size() + sizeof(llvm::ELF::Elf32_Sym));
              strtab.setSize(strtab.size() + stub->symInfo()->nameSize() + 1);
              isRelaxed = true;
            }
          }
          break;

          default:
            break;
        }
      }
    }
  }

  // find the first fragment w/ invalid offset due to stub insertion
  Fragment* invalid = NULL;
  pFinished = true;
  for (BranchIslandFactory::iterator island = getBRIslandFactory()->begin(),
       island_end = getBRIslandFactory()->end(); island != island_end; ++island)
  {
    if ((*island).end() == file_format->getText().getSectionData()->end())
      break;

    Fragment* exit = (*island).end();
    if (((*island).offset() + (*island).size()) > exit->getOffset()) {
      invalid = exit;
      pFinished = false;
      break;
    }
  }

  // reset the offset of invalid fragments
  while (NULL != invalid) {
    invalid->setOffset(invalid->getPrevNode()->getOffset() +
                       invalid->getPrevNode()->size());
    invalid = invalid->getNextNode();
  }

  // reset the size of .text
  if (isRelaxed) {
    file_format->getText().setSize(
      file_format->getText().getSectionData()->back().getOffset() +
      file_format->getText().getSectionData()->back().size());
  }
  return isRelaxed;
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
