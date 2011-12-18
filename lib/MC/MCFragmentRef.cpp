//===- MCFragmentRef.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/MCFragmentRef.h>
#include <mcld/MC/MCRegionFragment.h>
#include <cstring>

using namespace std;
using namespace mcld;

//==========================
// MCFragmentRef
MCFragmentRef::MCFragmentRef()
  : m_pFragment(NULL), m_Offset(0) {
}

MCFragmentRef::MCFragmentRef(llvm::MCFragment& pFrag,
                             MCFragmentRef::Offset pOffset)
  : m_pFragment(&pFrag), m_Offset(pOffset) {
}

MCFragmentRef::~MCFragmentRef()
{
  m_pFragment = NULL;
  m_Offset = 0;
}

void MCFragmentRef::assign(llvm::MCFragment& pFrag, MCFragmentRef::Offset pOffset)
{
  m_pFragment = &pFrag;
  m_Offset = pOffset;
}

void MCFragmentRef::memcpy(void* pDest, size_t pNBytes, Offset pOffset) const
{
  // check if the offset is still in a legal range.
  if (NULL == m_pFragment)
    return;
  unsigned int total_offset = m_Offset + pOffset;
  switch(m_pFragment->getKind()) {
    case llvm::MCFragment::FT_Inst: {
      llvm::MCInstFragment* inst_frag = static_cast<llvm::MCInstFragment*>(m_pFragment);
      unsigned int total_length = inst_frag->getCode().size();
      if (total_length < (total_offset+pNBytes))
        pNBytes = total_length - total_offset;

      std::memcpy(pDest, (inst_frag->getCode().data()+total_offset), pNBytes);
      return;
    }
    case llvm::MCFragment::FT_Data: {
      llvm::MCDataFragment* data_frag = static_cast<llvm::MCDataFragment*>(m_pFragment);
      unsigned int total_length = data_frag->getContents().size();
      if (total_length < (total_offset+pNBytes))
        pNBytes = total_length - total_offset;

      std::memcpy(pDest, (data_frag->getContents().data()+total_offset), pNBytes);
      return;
    }
    case llvm::MCFragment::FT_Region: {
      MCRegionFragment* region_frag = static_cast<mcld::MCRegionFragment*>(m_pFragment);
      unsigned int total_length = region_frag->getRegion().size();
      if (total_length < (total_offset+pNBytes))
        pNBytes = total_length - total_offset;

      std::memcpy(pDest, region_frag->getRegion().getBuffer(total_offset), pNBytes);
      return;
    }
    case llvm::MCFragment::FT_Align:
    case llvm::MCFragment::FT_Fill:
    case llvm::MCFragment::FT_Org:
    case llvm::MCFragment::FT_Dwarf:
    case llvm::MCFragment::FT_DwarfFrame:
    case llvm::MCFragment::FT_LEB:
    default:
      return;
  }
}

MCFragmentRef::Address MCFragmentRef::deref()
{
  if (NULL == m_pFragment)
    return NULL;
  Address base = NULL;
  switch(m_pFragment->getKind()) {
    case llvm::MCFragment::FT_Inst:
      base = (Address)static_cast<llvm::MCInstFragment*>(m_pFragment)->getCode().data();
      break;
    case llvm::MCFragment::FT_Data:
      base = (Address)static_cast<llvm::MCDataFragment*>(m_pFragment)->getContents().data();
      break;
    case llvm::MCFragment::FT_Region:
      base = static_cast<mcld::MCRegionFragment*>(m_pFragment)->getRegion().getBuffer();
      break;
    case llvm::MCFragment::FT_Align:
    case llvm::MCFragment::FT_Fill:
    case llvm::MCFragment::FT_Org:
    case llvm::MCFragment::FT_Dwarf:
    case llvm::MCFragment::FT_DwarfFrame:
    case llvm::MCFragment::FT_LEB:
    default:
      return NULL;
  }
  //cerr << base << endl;
  return base + m_Offset;
}

MCFragmentRef::ConstAddress MCFragmentRef::deref() const
{
  if (NULL == m_pFragment)
    return NULL;
  ConstAddress base = NULL;
  switch(m_pFragment->getKind()) {
    case llvm::MCFragment::FT_Inst:
      base = (ConstAddress)static_cast<const llvm::MCInstFragment*>(m_pFragment)->getCode().data();
      break;
    case llvm::MCFragment::FT_Data:
      base = (ConstAddress)static_cast<const llvm::MCDataFragment*>(m_pFragment)->getContents().data();
      break;
    case llvm::MCFragment::FT_Region:
      base = static_cast<const mcld::MCRegionFragment*>(m_pFragment)->getRegion().getBuffer();
      break;
    case llvm::MCFragment::FT_Align:
    case llvm::MCFragment::FT_Fill:
    case llvm::MCFragment::FT_Org:
    case llvm::MCFragment::FT_Dwarf:
    case llvm::MCFragment::FT_DwarfFrame:
    case llvm::MCFragment::FT_LEB:
    default:
      return NULL;
  }
  return base + m_Offset;
}

