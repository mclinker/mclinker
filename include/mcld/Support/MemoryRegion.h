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

#include <mcld/ADT/Uncopyable.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/Space.h>

namespace mcld
{

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
friend class RegionFactory;
friend class MemoryArea;

public:
  typedef Space::Address Address;
  typedef Space::ConstAddress ConstAddress;

private:
  MemoryRegion(Space& pParent, const Address pVMAStart, size_t pSize);

  Space* parent()
  { return &m_Parent; }

  const Space* parent() const
  { return &m_Parent; }

public:
  ~MemoryRegion();

  Address start()
  { return m_VMAStart; }

  ConstAddress start() const
  { return m_VMAStart; }

  Address end()
  { return m_VMAStart+m_Length; }

  ConstAddress end() const
  { return m_VMAStart+m_Length; }

  size_t size() const
  { return m_Length; }

  Address getBuffer(size_t pOffset = 0)
  { return m_VMAStart+pOffset; }

  ConstAddress getBuffer(size_t pOffset = 0) const
  { return m_VMAStart+pOffset; }
 
private:
  Space& m_Parent;
  Address m_VMAStart;
  size_t m_Length;
};

} // namespace of mcld

#endif

