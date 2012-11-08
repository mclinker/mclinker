//===- MemoryRegion.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LD_MEMORY_REGION_H
#define LD_MEMORY_REGION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Config/Config.h>
#include <mcld/ADT/Uncopyable.h>
#include <mcld/Support/Allocators.h>
#include <mcld/Support/Space.h>

namespace mcld {

class MemoryArea;

/** \class MemoryRegion
 *  \brief MemoryRegion is a range of virtual memory which is mapped onto a
 *  range of files which is opened by MemoryArea.
 *
 *  MemoryArea maps a file onto virtual memory. Clients can get a range of
 *  mapped memory space by requesting a MemoryRegion from MemoryArea, and
 *  read/write the mapped file through the MemoryRegion.
 *
 *  When two different MemoryRegion may overlap memory space, race condition
 *  may occurs. Clients must call MemoryRegion::sync() explicit to tell the
 *  MemoryArea when to synchronize the virtual memory space with the mapped
 *  file.
 */
class MemoryRegion : private Uncopyable
{
friend class Chunk<MemoryRegion, MCLD_REGION_CHUNK_SIZE>;
friend class RegionFactory;
friend class MemoryArea;

public:
  typedef Space::Address Address;
  typedef Space::ConstAddress ConstAddress;

private:
  MemoryRegion();

  MemoryRegion(Space& pParent, const Address pVMAStart, size_t pSize);

  ~MemoryRegion();

public:
  static MemoryRegion* Create(void* pStart, size_t pSize, Space& pSpace);

  static void Destroy(MemoryRegion*& pRegion);

  const Space* parent() const { return m_pParent; }
  Space*       parent()       { return m_pParent; }

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
  Space* m_pParent;
  Address m_VMAStart;
  size_t m_Length;
};

} // namespace of mcld

#endif

