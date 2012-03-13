//===- MemoryRegion.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/MemoryRegion.h>

using namespace mcld;

//==========================
// MemoryRegion
MemoryRegion::MemoryRegion(MemoryArea::Space *pParentSpace,
                           const MemoryRegion::Address pVMAStart,
                           size_t pSize)
  : m_pParentSpace(pParentSpace), m_VMAStart(pVMAStart), m_Length(pSize) {
  m_pParentSpace->region_num++;
}

MemoryRegion::~MemoryRegion()
{
  drift();
}

void MemoryRegion::drift()
{
  if (NULL == m_pParentSpace)
    return;
  m_pParentSpace->region_num--;
  m_pParentSpace = NULL;
}

