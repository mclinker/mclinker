//===- StrSymPool.cpp -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/LD/StrSymPool.h"

namespace mcld
{

//==========================
// StrSymPool
StrSymPool::StrSymPool(StrSymPool::size_type pSize)
  : m_Table(pSize) {
}

StrSymPool::~StrSymPool()
{
}

void StrSymPool::reserve(StrSymPool::size_type pSize)
{
  m_Table.rehash(pSize);
}

StrSymPool::size_type StrSymPool::capacity() const
{
  return (m_Table.numOfBuckets() - m_Table.numOfEntries());
}

} // namespace of mcld
