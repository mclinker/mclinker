//===- MemoryFactory.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ALONE_SUPPORT_MEMORY_FACTORY_H
#define ALONE_SUPPORT_MEMORY_FACTORY_H

#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MemoryAreaFactory.h>

namespace mcld {
class MemoryArea;
} // end namespace mcld

namespace alone {

class MemoryFactory : public mcld::MemoryAreaFactory {
public:
  MemoryFactory() : mcld::MemoryAreaFactory(32) { }

  ~MemoryFactory() { }

  using mcld::MemoryAreaFactory::produce;

  mcld::MemoryArea* produce(void *pMemBuffer, size_t pSize)
  { return mcld::MemoryAreaFactory::create(pMemBuffer, pSize); }

  mcld::MemoryArea* produce(int pFD)
  { return mcld::MemoryAreaFactory::create(pFD, mcld::FileHandle::Unknown); }
};

} // end namespace alone

#endif // ALONE_SUPPORT_MEMORY_FACTORY_H
