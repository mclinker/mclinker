//===- MemoryRegion.h -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
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

#include "mcld/ADT/Uncopyable.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/StringRef.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/MemoryArea.h"

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
typedef sys::fs::detail::Address Address;
private:
  MemoryRegion(MemoryArea::Space* pParentSpace,
               const Address pVMAStart,
               size_t pSize);
public:
  ~MemoryRegion();

  const Address start() const
  { return m_VMAStart; }

  // end is the next address of the last byte 
  const Address end() const
  { return m_VMAStart+m_Length; }

  size_t size() const
  { return m_Length; }

  const Address getBuffer() const
  { return m_VMAStart; }

  // sync - consist the memory space with the mapped file.
  void sync();

private:
  const Address m_VMAStart;
  size_t m_Length;
  MemoryArea::Space* m_pParentSpace;
};

} // namespace of mcld

#endif

