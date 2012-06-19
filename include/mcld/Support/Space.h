//===- Space.h ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MEMORY_SPACE_H
#define MCLD_MEMORY_SPACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/ilist_node.h>

namespace mcld
{

/** \class Space
 *  \brief Space contains a chunk of memory space that does not overlap with
 *  the other Space.
 */
class Space : public llvm::ilist_node<Space>
{
public:
  enum Type
  {
    ALLOCATED_ARRAY,
    MMAPED,
    EXTERNAL,
    UNALLOCATED
  };

public:
  explicit Space(Type pType, void* pMemBuffer, size_t pSize);

  ~Space();

  void setStart(size_t pOffset)
  { m_StartOffset = pOffset; }

  void* memory()
  { return m_Data; }

  const void* memory() const
  { return m_Data; }

  size_t start() const
  { return m_StartOffset; }

  size_t size() const
  { return m_Size; }

  void addRegion(MemoryRegion& pRegion)
  { ++m_RegionCount; }

  void removeRegion(MemoryRegion& pRegion)
  { --m_RegionCount; }

  size_t numOfRegions() const
  { return m_RegionCount; }

  Type type() const
  { return m_Type; }

public:
  void* m_Data;
  uint32_t m_StartOffset;
  uint32_t m_Size;
  uint16_t m_RegionCount;
  Type m_Type : 2;
};

} // namespace of mcld

#endif

