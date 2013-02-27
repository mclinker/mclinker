//===- FileHandle.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Config/Config.h"
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/FileSystem.h>
#include <errno.h>

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif
#if defined(HAVE_FCNTL_H)
# include <fcntl.h>
#endif

#include <sys/stat.h>
#include <sys/mman.h>

#if defined(_MSC_VER)
#include <io.h>
#include <fcntl.h>
#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif
#endif

using namespace mcld;

//===----------------------------------------------------------------------===//
// FileHandle
FileHandle::FileHandle()
  : m_Path(),
    m_Handler(-1),
    m_Size(0),
    m_State(GoodBit),
    m_OpenMode(NotOpen) {
}

FileHandle::~FileHandle()
{
  if (isOpened())
    close();
}

inline static int oflag(FileHandle::OpenMode pMode)
{
  int result = 0x0;
  if (FileHandle::Unknown == pMode)
    return result;

  if (FileHandle::ReadWrite == (pMode & FileHandle::ReadWrite))
    result |= O_RDWR;
  else if (FileHandle::ReadOnly == (pMode & FileHandle::ReadOnly))
    result |= O_RDONLY;
  else if (FileHandle::WriteOnly == (pMode & FileHandle::WriteOnly))
    result |= O_WRONLY;

  if (FileHandle::Append == (pMode & FileHandle::Append))
    result |= O_APPEND;

  if (FileHandle::Create == (pMode & FileHandle::Create))
    result |= O_CREAT;

  if (FileHandle::Truncate == (pMode & FileHandle::Truncate))
    result |= O_TRUNC;

  return result;
}

inline static bool get_size(int pHandler, unsigned int &pSize)
{
  struct ::stat file_stat;
  if (-1 == ::fstat(pHandler, &file_stat)) {
    pSize = 0;
    return false;
  }
  pSize = file_stat.st_size;
  return true;
}

bool FileHandle::open(const sys::fs::Path& pPath,
                      FileHandle::OpenMode pMode,
                      FileHandle::Permission pPerm)
{
  if (isOpened() || Unknown == pMode) {
    setState(BadBit);
    return false;
  }

  m_OpenMode = pMode;
  if (System == pPerm)
    m_Handler = sys::fs::detail::open(pPath, oflag(pMode));
  else
    m_Handler = sys::fs::detail::open(pPath, oflag(pMode), (int)pPerm);

  m_Path = pPath;
  if (-1 == m_Handler) {
    m_OpenMode = NotOpen;
    setState(FailBit);
    return false;
  }

  if (!get_size(m_Handler, m_Size)) {
    setState(FailBit);
    return false;
  }

  return true;
}

bool FileHandle::delegate(int pFD, FileHandle::OpenMode pMode)
{
  if (isOpened()) {
    setState(BadBit);
    return false;
  }

  m_Handler = pFD;
  m_OpenMode = pMode;
  m_State = (GoodBit | DeputedBit);

  if (!get_size(m_Handler, m_Size)) {
    setState(FailBit);
    return false;
  }

  return true;
}

bool FileHandle::close()
{
  if (!isOpened()) {
    setState(BadBit);
    return false;
  }

  if (isOwned()) {
    if (-1 == ::close(m_Handler)) {
      setState(FailBit);
      return false;
    }
  }

  m_Path.native().clear();
  m_Size = 0;
  m_OpenMode = NotOpen;
  cleanState();
  return true;
}

bool FileHandle::truncate(size_t pSize)
{
  if (!isOpened() || !isWritable()) {
    setState(BadBit);
    return false;
  }

  if (-1 == sys::fs::detail::ftruncate(m_Handler, pSize)) {
    setState(FailBit);
    return false;
  }

  m_Size = pSize;
  return true;
}

bool FileHandle::read(void* pMemBuffer, size_t pStartOffset, size_t pLength)
{
  if (!isOpened() || !isReadable()) {
    setState(BadBit);
    return false;
  }

  if (0 == pLength)
    return true;

  ssize_t read_bytes = sys::fs::detail::pread(m_Handler,
                                              pMemBuffer,
                                              pLength,
                                              pStartOffset);

  if (-1 == read_bytes) {
    setState(FailBit);
    return false;
  }

  return true;
}

bool FileHandle::write(const void* pMemBuffer, size_t pStartOffset, size_t pLength)
{
  if (!isOpened() || !isWritable()) {
    setState(BadBit);
    return false;
  }

  if (0 == pLength)
    return true;


  ssize_t write_bytes = sys::fs::detail::pwrite(m_Handler,
                                                pMemBuffer,
                                                pLength,
                                                pStartOffset);

  if (-1 == write_bytes) {
    setState(FailBit);
    return false;
  }

  return true;
}

bool FileHandle::mmap(void*& pMemBuffer, size_t pStartOffset, size_t pLength)
{
  if (!isOpened()) {
    setState(BadBit);
    return false;
  }

  if (0 == pLength)
    return true;

  int prot, flag;
  if (isReadable() && !isWritable()) {
    // read-only
    prot = PROT_READ;
    flag = MAP_FILE | MAP_PRIVATE;
  }
  else if (!isReadable() && isWritable()) {
    // write-only
    prot = PROT_WRITE;
    flag = MAP_FILE | MAP_SHARED;
  }
  else if (isReadWrite()) {
    // read and write
    prot = PROT_READ | PROT_WRITE;
    flag = MAP_FILE | MAP_SHARED;
  }
  else {
    // can not read/write
    setState(BadBit);
    return false;
  }

  pMemBuffer = ::mmap(NULL, pLength, prot, flag, m_Handler, pStartOffset);

  if (MAP_FAILED == pMemBuffer) {
    setState(FailBit);
    return false;
  }

  return true;
}

bool FileHandle::munmap(void* pMemBuffer, size_t pLength)
{
  if (!isOpened()) {
    setState(BadBit);
    return false;
  }

  if (-1 == ::munmap(pMemBuffer, pLength)) {
    setState(FailBit);
    return false;
  }

  return true;
}

void FileHandle::setState(FileHandle::IOState pState)
{
  m_State |= pState;
}

void FileHandle::cleanState(FileHandle::IOState pState)
{
  m_State = pState;
}

bool FileHandle::isOpened() const
{
  if (-1 != m_Handler && m_OpenMode != NotOpen && isGood())
    return true;

  return false;
}

// Assume Unknown OpenMode is readable
bool FileHandle::isReadable() const
{
  return (m_OpenMode & ReadOnly);
}

// Assume Unknown OpenMode is writable
bool FileHandle::isWritable() const
{
  return (m_OpenMode & WriteOnly);
}

// Assume Unknown OpenMode is both readable and writable
bool FileHandle::isReadWrite() const
{
  return (FileHandle::ReadWrite == (m_OpenMode & FileHandle::ReadWrite));
}

bool FileHandle::isGood() const
{
  return !(m_State & (BadBit | FailBit));
}

bool FileHandle::isBad() const
{
  return (m_State & BadBit);
}

bool FileHandle::isFailed() const
{
  return (m_State & (BadBit | FailBit));
}

bool FileHandle::isOwned() const
{
  return !(m_State & DeputedBit);
}

