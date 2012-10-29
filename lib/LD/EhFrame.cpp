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
#include <mcld/LD/Layout.h>
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
                  uint32_t pFileOffset,
                  uint32_t pDataStart)
  : RegionFragment(pRegion),
    m_CIE(pCIE),
    m_FileOffset(pFileOffset),
    m_DataStart(pDataStart) {
}

//===----------------------------------------------------------------------===//
// EhFrame
//===----------------------------------------------------------------------===//
EhFrame::EhFrame(LDSection& pSection, Layout& pLayout)
  : m_Section(pSection), m_Layout(pLayout) {
}

EhFrame::~EhFrame()
{
  // Do nothing. Since all CIEs, FDEs and regular fragments are stored in
  // iplist, iplist will delete the fragments and we do not need to handle with
  // it.
}

void EhFrame::addFragment(RegionFragment& pFrag, LDSection& pSection)
{
  size_t size =  m_Layout.appendFragment(pFrag,
                                         *m_Section.getSectionData(),
                                         pSection.align());

  m_Section.setSize(m_Section.size() + size);
}

void EhFrame::addCIE(EhFrame::CIE& pCIE)
{
  m_CIEs.push_back(&pCIE);
}

void EhFrame::addFDE(EhFrame::FDE& pFDE)
{
  m_FDEs.push_back(&pFDE);
}

