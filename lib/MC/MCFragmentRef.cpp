/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCFragmentRef.h>
#include <mcld/MC/MCRegionFragment.h>
#include <iostream>

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

MCFragmentRef::Address MCFragmentRef::deref()
{
  if (NULL == m_pFragment)
    return NULL;
  Address base = NULL;
  switch(m_pFragment->getKind()) {
    case llvm::MCFragment::FT_Inst:
      base = static_cast<llvm::MCInstFragment*>(m_pFragment)->getCode().data();
      break;
    case llvm::MCFragment::FT_Data:
      base = static_cast<llvm::MCDataFragment*>(m_pFragment)->getContents().data();
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
  cerr << base << endl;
  return base + m_Offset;
}

const MCFragmentRef::Address MCFragmentRef::deref() const
{
  if (NULL == m_pFragment)
    return NULL;
  Address base = NULL;
  switch(m_pFragment->getKind()) {
    case llvm::MCFragment::FT_Inst:
      base = static_cast<const llvm::MCInstFragment*>(m_pFragment)->getCode().data();
      break;
    case llvm::MCFragment::FT_Data:
      base = static_cast<const llvm::MCDataFragment*>(m_pFragment)->getContents().data();
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

