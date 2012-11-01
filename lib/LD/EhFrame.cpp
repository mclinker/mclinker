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

void EhFrame::addCIE(EhFrame::cie_iterator pFirst, EhFrame::cie_iterator pLast)
{
  // cie_iterator is random access iterator, so we can substract them to get
  // the number of elements
  size_t size = pLast - pFirst;
  m_CIEs.reserve(size + m_CIEs.size());
  for (cie_iterator cie = pFirst; cie != pLast; ++cie) {
    m_CIEs.push_back(*cie);
  }
}

void EhFrame::addFDE(EhFrame::FDE& pFDE)
{
  m_FDEs.push_back(&pFDE);
}

void EhFrame::addFDE(EhFrame::fde_iterator pFirst, EhFrame::fde_iterator pLast)
{
  // fde_iterator is random access iterator, so we can substract them to get
  // the number of elements
  size_t size = pLast - pFirst;
  m_FDEs.reserve(size + m_FDEs.size());
  for (fde_iterator fde = pFirst; fde != pLast; ++fde) {
    m_FDEs.push_back(*fde);
  }
}

