//===- MemoryArea.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/MemoryArea.h>
#include <llvm/Support/system_error.h>

#include <cassert>

using namespace mcld;

//===--------------------------------------------------------------------===//
// MemoryArea
//===--------------------------------------------------------------------===//
MemoryArea::MemoryArea(llvm::StringRef pFilename)
{
  llvm::MemoryBuffer::getFile(pFilename,
                              m_pMemoryBuffer,
                              /*FileSize*/ -1,
                              /*RequiresNullTerminator*/ false);
}

MemoryArea::MemoryArea(const char* pMemBuffer, size_t pSize)
{
  llvm::StringRef mem(pMemBuffer, pSize);
  llvm::MemoryBuffer* buffer =
      llvm::MemoryBuffer::getMemBuffer(mem, /*BufferName*/ "NaN",
                                       /*RequiresNullTerminator*/ false);
  assert(buffer != NULL);
  m_pMemoryBuffer.reset(buffer);
}

llvm::StringRef MemoryArea::request(size_t pOffset, size_t pLength)
{
  return llvm::StringRef(m_pMemoryBuffer->getBufferStart() + pOffset, pLength);
}

size_t MemoryArea::size() const
{
  return m_pMemoryBuffer->getBufferSize();
}
