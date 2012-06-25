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
#include <llvm/Support/DataTypes.h>
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <mcld/ADT/TypeTraits.h>

namespace mcld
{

class FileHandle;
class MemoryRegion;

/** \class Space
 *  \brief Space contains a chunk of memory space that does not overlap with
 *  the other Space.
 *
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

  typedef NonConstTraits<uint8_t>::pointer Address;
  typedef ConstTraits<uint8_t>::pointer ConstAddress;

// llvm::iplist functions
public:
  // llvm::iplist needs default constructor to make a sentinel.
  // Normal users should use @ref Space::createSpace function.
  Space();

  // llvm::iplist needs public destructor to delete the sentinel.
  // Normal users should use @ref Space::releaseSpace function.
  ~Space();

  // This constructor is opened for the clients who want to control the
  // details. In MCLinker, this constructor is used no where.
  Space(Type pType, void* pMemBuffer, size_t pSize);
  
public:
  void setStart(size_t pOffset)
  { m_StartOffset = pOffset; }

  Address memory()
  { return m_Data; }

  ConstAddress memory() const
  { return m_Data; }

  size_t start() const
  { return m_StartOffset; }

  size_t size() const
  { return m_Size; }

  Type type() const
  { return m_Type; }

  void addRegion(MemoryRegion& pRegion)
  { ++m_RegionCount; }

  void removeRegion(MemoryRegion& pRegion)
  { --m_RegionCount; }

  size_t numOfRegions() const
  { return m_RegionCount; }

  static Space* createSpace(FileHandle& pHandler,
                            size_t pOffset, size_t pSize);
  
  static void releaseSpace(Space* pSpace, FileHandle& pHandler);

  static void syncSpace(Space* pSpace, FileHandle& pHandler);

private:
  Address m_Data;
  uint32_t m_StartOffset;
  uint32_t m_Size;
  uint16_t m_RegionCount;
  Type m_Type : 2;
};

} // namespace of mcld

#endif

