//===- Fragment.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/Fragment.h>

#include <llvm/Support/DataTypes.h>

#include <mcld/LD/SectionData.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Fragment
//===----------------------------------------------------------------------===//
Fragment::Fragment()
  : m_Kind(Type(~0)), m_pParent(NULL), m_Offset(~uint64_t(0)), m_LayoutOrder(~(0U)) {
}

Fragment::Fragment(Type pKind, SectionData *pParent)
  : m_Kind(pKind), m_pParent(pParent), m_Offset(~uint64_t(0)), m_LayoutOrder(~(0U)) {
  if (NULL != m_pParent)
    m_pParent->getFragmentList().push_back(this);
}

Fragment::~Fragment()
{
}

