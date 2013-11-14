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
#include <mcld/Object/ObjectBuilder.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/GCFactory.h>

#include <llvm/Support/ManagedStatic.h>

using namespace mcld;

typedef GCFactory<EhFrame, MCLD_SECTIONS_PER_INPUT> EhFrameFactory;

static llvm::ManagedStatic<EhFrameFactory> g_EhFrameFactory;

//===----------------------------------------------------------------------===//
// EhFrame::Record
//===----------------------------------------------------------------------===//
EhFrame::Record::Record(MemoryRegion& pRegion)
  : RegionFragment(pRegion) {
}

EhFrame::Record::~Record()
{
  // llvm::iplist will manage and delete the fragments
}

//===----------------------------------------------------------------------===//
// EhFrame::CIE
//===----------------------------------------------------------------------===//
EhFrame::CIE::CIE(MemoryRegion& pRegion)
  : EhFrame::Record(pRegion),
    m_FDEEncode(0u), m_Mergeable(false), m_pReloc(0), m_PersonalityOffset(0) {
}

EhFrame::CIE::~CIE()
{
}

//===----------------------------------------------------------------------===//
// EhFrame::FDE
//===----------------------------------------------------------------------===//
EhFrame::FDE::FDE(MemoryRegion& pRegion, EhFrame::CIE& pCIE)
  : EhFrame::Record(pRegion), m_pCIE(&pCIE) {
}

EhFrame::FDE::~FDE()
{
}

void EhFrame::FDE::setCIE(EhFrame::CIE& pCIE)
{
  m_pCIE = &pCIE;
  m_pCIE->add(*this);
}

//===----------------------------------------------------------------------===//
// EhFrame::GeneratedCIE
//===----------------------------------------------------------------------===//
EhFrame::GeneratedCIE::GeneratedCIE(MemoryRegion& pRegion)
  : EhFrame::CIE(pRegion) {
}

EhFrame::GeneratedCIE::~GeneratedCIE()
{
}

//===----------------------------------------------------------------------===//
// EhFrame::GeneratedFDE
//===----------------------------------------------------------------------===//
EhFrame::GeneratedFDE::GeneratedFDE(MemoryRegion& pRegion, CIE &pCIE)
  : EhFrame::FDE(pRegion, pCIE) {
}

EhFrame::GeneratedFDE::~GeneratedFDE()
{
}

//===----------------------------------------------------------------------===//
// EhFrame
//===----------------------------------------------------------------------===//
EhFrame::DataStartOffset EhFrame::g_Offset = EhFrame::OFFSET_32BIT;

EhFrame::EhFrame()
  : m_pSection(NULL), m_pSectionData(NULL) {
}

EhFrame::EhFrame(LDSection& pSection)
  : m_pSection(&pSection),
    m_pSectionData(NULL) {
  m_pSectionData = SectionData::Create(pSection);
}

EhFrame::~EhFrame()
{
}

EhFrame* EhFrame::Create(LDSection& pSection)
{
  EhFrame* result = g_EhFrameFactory->allocate();
  new (result) EhFrame(pSection);
  return result;
}

void EhFrame::Destroy(EhFrame*& pSection)
{
  pSection->~EhFrame();
  g_EhFrameFactory->deallocate(pSection);
  pSection = NULL;
}

void EhFrame::Clear()
{
  g_EhFrameFactory->clear();
}

const LDSection& EhFrame::getSection() const
{
  assert(NULL != m_pSection);
  return *m_pSection;
}

LDSection& EhFrame::getSection()
{
  assert(NULL != m_pSection);
  return *m_pSection;
}

void EhFrame::addFragment(Fragment& pFrag)
{
  uint32_t offset = 0;
  if (!m_pSectionData->empty())
    offset = m_pSectionData->back().getOffset() + m_pSectionData->back().size();

  m_pSectionData->getFragmentList().push_back(&pFrag);
  pFrag.setParent(m_pSectionData);
  pFrag.setOffset(offset);
}

void EhFrame::addCIE(EhFrame::CIE& pCIE, bool pAlsoAddFragment)
{
  m_CIEs.push_back(&pCIE);
  if (pAlsoAddFragment)
    addFragment(pCIE);
}

void EhFrame::addFDE(EhFrame::FDE& pFDE, bool pAlsoAddFragment)
{
  pFDE.getCIE().add(pFDE);
  if (pAlsoAddFragment)
    addFragment(pFDE);
}

size_t EhFrame::numOfFDEs() const
{
  // FDE number only used by .eh_frame_hdr computation, and the number of CIE
  // is usually not too many. It is worthy to compromise space by time
  size_t size = 0u;
  for (const_cie_iterator i = cie_begin(), e = cie_end(); i != e; ++i)
    size += (*i)->numOfFDEs();
  return size;
}

EhFrame& EhFrame::merge(EhFrame& pOther)
{
  ObjectBuilder::MoveSectionData(*pOther.getSectionData(), *m_pSectionData);

  // TODO: The CIEs should be merged as many as possible.
  m_CIEs.reserve(pOther.numOfCIEs() + m_CIEs.size());
  for (cie_iterator cie = pOther.cie_begin(); cie != pOther.cie_end(); ++cie)
    m_CIEs.push_back(*cie);
  pOther.m_CIEs.clear();
  return *this;
}

//===----------------------------------------------------------------------===//
// operator
//===----------------------------------------------------------------------===//
bool mcld::operator<(const EhFrame::CIE& p1, const EhFrame::CIE& p2)
{
  const std::string& pr1 = p1.getPersonalityName();
  const std::string& pr2 = p2.getPersonalityName();
  if (pr1 != pr2)
    return pr1 < pr2;
  return p1.getAugmentationData() < p2.getAugmentationData();
}
