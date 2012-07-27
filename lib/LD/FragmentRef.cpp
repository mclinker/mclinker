//===- FragmentRef.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/FragmentRef.h>

#include <cstring>
#include <cassert>

#include <llvm/Support/MathExtras.h>
#include <llvm/Support/Casting.h>

#include <mcld/LD/AlignFragment.h>
#include <mcld/LD/FillFragment.h>
#include <mcld/LD/RegionFragment.h>
#include <mcld/LD/TargetFragment.h>
#include <mcld/LD/Layout.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Helper Functions
//===----------------------------------------------------------------------===//
/// compunteFragmentSize - compute the specific Fragment size
uint64_t mcld::computeFragmentSize(const Layout& pLayout,
                                   const Fragment& pFrag)
{
  switch (pFrag.getKind()) {
    case Fragment::Fillment:
      return static_cast<const FillFragment&>(pFrag).getSize();

    case Fragment::Alignment: {
      uint64_t offset = pLayout.getOutputOffset(pFrag);
      const AlignFragment& align_frag = llvm::cast<AlignFragment>(pFrag);
      uint64_t size = llvm::OffsetToAlignment(offset, align_frag.getAlignment());
      if (size > align_frag.getMaxBytesToEmit())
        return 0;
      return size;
    }

    case Fragment::Region:
      return llvm::cast<RegionFragment>(pFrag).getRegion().size();

    case Fragment::Target:
      return llvm::cast<TargetFragment>(pFrag).getSize();

    case Fragment::Relocation:
      assert(0 && "the size of FT_Reloc fragment is handled by backend");
      return 0;

    default:
      assert(0 && "invalid fragment kind");
      return 0;
  }
}

//===----------------------------------------------------------------------===//
// FragmentRef
//===----------------------------------------------------------------------===//
FragmentRef::FragmentRef()
  : m_pFragment(NULL), m_Offset(0) {
}

FragmentRef::FragmentRef(Fragment& pFrag,
                         FragmentRef::Offset pOffset)
  : m_pFragment(&pFrag), m_Offset(pOffset) {
}

FragmentRef::~FragmentRef()
{
  m_pFragment = NULL;
  m_Offset = 0;
}

FragmentRef& FragmentRef::assign(const FragmentRef& pCopy)
{
  m_pFragment = const_cast<Fragment*>(pCopy.m_pFragment);
  m_Offset = pCopy.m_Offset;
  return *this;
}

FragmentRef& FragmentRef::assign(Fragment& pFrag, FragmentRef::Offset pOffset)
{
  m_pFragment = &pFrag;
  m_Offset = pOffset;
  return *this;
}

void FragmentRef::memcpy(void* pDest, size_t pNBytes, Offset pOffset) const
{
  // check if the offset is still in a legal range.
  if (NULL == m_pFragment)
    return;
  unsigned int total_offset = m_Offset + pOffset;
  switch(m_pFragment->getKind()) {
    case Fragment::Region: {
      RegionFragment* region_frag = static_cast<RegionFragment*>(m_pFragment);
      unsigned int total_length = region_frag->getRegion().size();
      if (total_length < (total_offset+pNBytes))
        pNBytes = total_length - total_offset;

      std::memcpy(pDest, region_frag->getRegion().getBuffer(total_offset), pNBytes);
      return;
    }
    case Fragment::Alignment:
    case Fragment::Fillment:
    default:
      return;
  }
}

FragmentRef::Address FragmentRef::deref()
{
  if (NULL == m_pFragment)
    return NULL;
  Address base = NULL;
  switch(m_pFragment->getKind()) {
    case Fragment::Region:
      base = static_cast<RegionFragment*>(m_pFragment)->getRegion().getBuffer();
      break;
    case Fragment::Alignment:
    case Fragment::Fillment:
    default:
      return NULL;
  }
  return base + m_Offset;
}

FragmentRef::ConstAddress FragmentRef::deref() const
{
  if (NULL == m_pFragment)
    return NULL;
  ConstAddress base = NULL;
  switch(m_pFragment->getKind()) {
    case Fragment::Region:
      base = static_cast<const RegionFragment*>(m_pFragment)->getRegion().getBuffer();
      break;
    case Fragment::Alignment:
    case Fragment::Fillment:
    default:
      return NULL;
  }
  return base + m_Offset;
}

