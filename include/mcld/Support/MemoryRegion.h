//===- MemoryRegion.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_MEMORYREGION_H
#define MCLD_SUPPORT_MEMORYREGION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Config/Config.h>
#include <mcld/ADT/Uncopyable.h>
#include <mcld/ADT/TypeTraits.h>
#include <mcld/Support/Allocators.h>

#include <llvm/Support/DataTypes.h>

namespace mcld {

class MemoryArea;

/** \class MemoryRegion
 *  \brief MemoryRegion is a range of virtual memory which is mapped onto a
 *  range of files which is opened by MemoryArea, which is only used for
 *  FileOutputBuffer manageement now.
 *
 *  MemoryArea maps a file onto virtual memory. Clients can get a range of
 *  mapped memory space by requesting a MemoryRegion from FileOutputBuffer, and
 *  read/write the mapped file through the MemoryRegion.
 */
class MemoryRegion : private Uncopyable
{
friend class Chunk<MemoryRegion, MCLD_REGION_CHUNK_SIZE>;
friend class RegionFactory;

public:
  typedef NonConstTraits<uint8_t>::pointer Address;
  typedef ConstTraits<uint8_t>::pointer ConstAddress;

private:
  MemoryRegion();

  MemoryRegion(const Address pVMAStart, size_t pSize);

  ~MemoryRegion();

public:
  /// Create - To wrap a piece of memory and to create a new region.
  /// This function wraps a piece of memory and to create a new region. Region
  /// is just a wraper, it is not responsible for deallocate the given memory.
  ///
  /// @param pStart [in] The start address of a piece of memory
  /// @param pSize  [in] The size of the given memory
  static MemoryRegion* Create(void* pStart, size_t pSize);

  /// Destroy - To destroy the region
  /// If the region has a parent space, it will be also remove from the space.
  ///
  /// @param pRegion [in, out] pRegion is set to NULL if the destruction is
  /// success.
  static void Destroy(MemoryRegion*& pRegion);

  ConstAddress start() const { return m_VMAStart; }
  Address      start()       { return m_VMAStart; }

  ConstAddress end() const { return m_VMAStart+m_Length; }
  Address      end()       { return m_VMAStart+m_Length; }

  size_t size() const { return m_Length; }

  ConstAddress getBuffer(size_t pOffset = 0) const
  { return m_VMAStart+pOffset; }
 
  Address getBuffer(size_t pOffset = 0)
  { return m_VMAStart+pOffset; }

private:
  Address m_VMAStart;
  size_t m_Length;
};

} // namespace of mcld

#endif

