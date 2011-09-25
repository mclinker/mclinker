/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Support/RegionFactory.h>

using namespace mcld;

//==========================
// MemoryAreaFactory
MemoryAreaFactory::MemoryAreaFactory(size_t pNum)
  : UniqueGCFactoryBase<sys::fs::Path, MemoryArea, 0>(pNum) {
  // For each loaded file, MCLinker must load ELF header, section header, 
  // symbol table, and string table. So, we set the size of chunk quadruple
  // larger than the number of input files.
  m_pRegionFactory = new RegionFactory(pNum*4);
}

MemoryAreaFactory::~MemoryAreaFactory()
{
  delete m_pRegionFactory;
}

MemoryArea* MemoryAreaFactory::produce(const sys::fs::Path& pPath, int pFlags)
{
  MemoryArea* result = find(pPath);
  if (0 == result) {
    result = allocate();
    new (result) MemoryArea(*m_pRegionFactory);
    result->map(pPath, pFlags);
  }
  f_KeyMap.insert(std::make_pair(pPath, result));
  return result;
}

MemoryArea* MemoryAreaFactory::produce(const sys::fs::Path& pPath, int pFlags, mode_t pMode)
{
  MemoryArea* result = find(pPath);
  if (0 == result) {
    result = allocate();
    new (result) MemoryArea(*m_pRegionFactory);
    result->map(pPath, pFlags, pMode);
  }
  f_KeyMap.insert(std::make_pair(pPath, result));
  return result;
}

