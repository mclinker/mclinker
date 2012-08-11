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

//===----------------------------------------------------------------------===//
// MemoryRegion
//===----------------------------------------------------------------------===//
MemoryRegion::MemoryRegion()
  : m_pParent(NULL), m_VMAStart(0), m_Length(0) {
}

MemoryRegion::MemoryRegion(Space& pParent,
                           MemoryRegion::Address pVMAStart,
                           size_t pSize)
  : m_pParent(&pParent), m_VMAStart(pVMAStart), m_Length(pSize) {
}

MemoryRegion::~MemoryRegion()
{
}

