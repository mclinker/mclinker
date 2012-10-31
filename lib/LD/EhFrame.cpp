//===- EhFrame.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Support/MemoryRegion.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// EhFrame::CIE
//===----------------------------------------------------------------------===//
EhFrame::CIE::CIE(MemoryRegion& pRegion)
  : RegionFragment(pRegion) {
}

//===----------------------------------------------------------------------===//
// EhFrame::FDE
//===----------------------------------------------------------------------===//
EhFrame::FDE::FDE(MemoryRegion& pRegion,
                  const EhFrame::CIE& pCIE,
                  uint32_t pDataStart)
  : RegionFragment(pRegion),
    m_CIE(pCIE),
    m_DataStart(pDataStart) {
}

//===----------------------------------------------------------------------===//
// EhFrame
//===----------------------------------------------------------------------===//
EhFrame::EhFrame(LDSection& pSection)
  : m_Section(pSection),
    m_pSectionData(NULL) {
  m_pSectionData = SectionData::Create(pSection);
}

EhFrame::~EhFrame()
{
  // Since all CIEs, FDEs and regular fragments are stored in iplist, iplist
  // will delete the fragments and we do not need to handle with it.
  SectionData::Destroy(m_pSectionData);
}

void EhFrame::addFragment(RegionFragment& pFrag)
{
  uint32_t offset = 0;
  if (!m_pSectionData->empty())
    offset = m_pSectionData->back().getOffset() + m_pSectionData->back().size();

  m_pSectionData->getFragmentList().push_back(&pFrag);
  pFrag.setOffset(offset);
}

void EhFrame::addCIE(EhFrame::CIE& pCIE)
{
  m_CIEs.push_back(&pCIE);
}

void EhFrame::addFDE(EhFrame::FDE& pFDE)
{
  m_FDEs.push_back(&pFDE);
}

