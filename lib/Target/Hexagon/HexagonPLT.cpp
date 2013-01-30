//===- HexagonPLT.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "HexagonPLT.h"

#include <llvm/Support/ELF.h>
#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// PLT entry data
//===----------------------------------------------------------------------===//
HexagonDynPLT0::HexagonDynPLT0(SectionData& pParent)
  : PLT::Entry<sizeof(hexagon_dyn_plt0)>(pParent)
{
}

HexagonExecPLT0::HexagonExecPLT0(SectionData& pParent)
  : PLT::Entry<sizeof(hexagon_exec_plt0)>(pParent)
{
}

//===----------------------------------------------------------------------===//
// HexagonPLT
//===----------------------------------------------------------------------===//
HexagonPLT::HexagonPLT(LDSection& pSection,
               HexagonGOT &pGOTPLT,
               const LinkerConfig& pConfig)
  : PLT(pSection),
    m_GOT(pGOTPLT),
    m_Config(pConfig)
{
  assert(LinkerConfig::DynObj == m_Config.codeGenType() ||
         LinkerConfig::Exec   == m_Config.codeGenType() ||
         LinkerConfig::Binary == m_Config.codeGenType());

  if (LinkerConfig::DynObj == m_Config.codeGenType()) {
    m_PLT0 = hexagon_dyn_plt0;
    m_PLT0Size = sizeof (hexagon_dyn_plt0);
    // create PLT0
    new HexagonDynPLT0(*m_SectionData);
  }
  else {
    m_PLT0 = hexagon_exec_plt0;
    m_PLT0Size = sizeof (hexagon_exec_plt0);
    // create PLT0
    new HexagonExecPLT0(*m_SectionData);
  }
  m_Last = m_SectionData->begin();
}

HexagonPLT::~HexagonPLT()
{
}

PLTEntryBase* HexagonPLT::getPLT0() const
{
  iterator first = m_SectionData->getFragmentList().begin();

  assert(first != m_SectionData->getFragmentList().end() &&
         "FragmentList is empty, getPLT0 failed!");

  PLTEntryBase* plt0 = &(llvm::cast<PLTEntryBase>(*first));

  return plt0;
}

void HexagonPLT::finalizeSectionSize()
{
  uint64_t size = 0;
  // plt0 size
  size = getPLT0()->size();

  m_Section.setSize(size);

  uint32_t offset = 0;
  SectionData::iterator frag, fragEnd = m_SectionData->end();
  for (frag = m_SectionData->begin(); frag != fragEnd; ++frag) {
    frag->setOffset(offset);
    offset += frag->size();
  }
}

void HexagonPLT::reserveEntry(size_t pNum)
{
}

