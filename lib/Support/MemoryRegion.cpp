//===- MemoryRegion.cpp ---------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/MemoryRegion.h"

using namespace mcld;

//==========================
// MemoryRegion
MemoryRegion::MemoryRegion(MemoryArea::Space *pParentSpace,
                           const MemoryRegion::Address pVMAStart,
                           size_t pSize)
  : m_pParentSpace(pParentSpace), m_VMAStart(pVMAStart), m_Length(pSize) {
}

MemoryRegion::~MemoryRegion()
{
}

void MemoryRegion::sync()
{
  m_pParentSpace->sync();
}

