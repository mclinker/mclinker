//===- MemoryRegion.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/RegionFactory.h>

#include <llvm/Support/ManagedStatic.h>

using namespace mcld;

static llvm::ManagedStatic<RegionFactory> g_RegionFactory;

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

MemoryRegion* MemoryRegion::Create(void* pStart, size_t pSize, Space& pSpace)
{
  return g_RegionFactory->produce(pSpace, static_cast<Address>(pStart), pSize);
}

void MemoryRegion::Destroy(MemoryRegion*& pRegion)
{
  g_RegionFactory->destruct(pRegion);
}

