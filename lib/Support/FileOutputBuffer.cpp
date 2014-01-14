//===- FileOutputBuffer.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/FileOutputBuffer.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/Path.h>

using namespace mcld;
using llvm::sys::fs::mapped_file_region;

FileOutputBuffer::FileOutputBuffer(llvm::sys::fs::mapped_file_region* pRegion,
                                   FileHandle& pFileHandle)
  : m_pRegion(pRegion), m_FileHandle(pFileHandle)
{
}

FileOutputBuffer::~FileOutputBuffer()
{
  // Unmap buffer, letting OS flush dirty pages to file on disk.
  m_pRegion.reset(0);
}

llvm::error_code FileOutputBuffer::create(FileHandle& pFileHandle,
    size_t pSize, llvm::OwningPtr<FileOutputBuffer>& pResult)
{
  llvm::error_code EC;
  llvm::OwningPtr<mapped_file_region> mapped_file(new mapped_file_region(
      pFileHandle.handler(),
      false,
      mapped_file_region::readwrite,
      pSize,
      0,
      EC));

  if (EC)
    return EC;

  pResult.reset(new FileOutputBuffer(mapped_file.get(), pFileHandle));
  if (pResult)
    mapped_file.take();
  return llvm::error_code::success();
}

MemoryRegion FileOutputBuffer::request(size_t pOffset, size_t pLength)
{
  if (pOffset > getBufferSize() || (pOffset + pLength) > getBufferSize())
    return MemoryRegion();
  return MemoryRegion(getBufferStart() + pOffset, pLength);
}

llvm::StringRef FileOutputBuffer::getPath() const
{
  return m_FileHandle.path().native();
}
