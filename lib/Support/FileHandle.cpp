//===- FileHandle.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MsgHandling.h>
#include <errno.h>
#include <fcntl.h>

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

inline static int oflag(enum FileHandle::OpenMode pMode)
{
  int result = 0x0;
  if (FileHandle::Unknown == pMode)
    return result;

  if (FileHandle::ReadWrite == (pMode & FileHandle::ReadWrite))
    result |= O_RDWR;
  else if (pMode & FileHandle::ReadOnly)
    result |= O_RDONLY;
  else if (pMode & FileHandle::WriteOnly)
    result |= O_WRONLY;

  if (pMode & FileHandle::Append)
    result |= O_APPEND;

  if (pMode & FileHandle::Create)
    result |= O_CREAT;

  if (pMode & FileHandle::Truncate)
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
                      enum FileHandle::OpenMode pMode)
{
  if (isOpened() || Unknown == pMode) {
    setState(BadBit);
    return false;
  }

  m_OpenMode = pMode;
  m_Handler = ::open(pPath.native().c_str(), oflag(pMode));
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

bool FileHandle::open(const sys::fs::Path& pPath,
                      enum FileHandle::OpenMode pMode,
                      enum FileHandle::Permission pPerm)
{
  if (isOpened() || Unknown == pMode) {
    setState(BadBit);
    return false;
  }

  m_OpenMode = pMode;
  m_Handler = ::open(pPath.native().c_str(), oflag(pMode), (int)pPerm);
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

bool FileHandle::delegate(int pFD, enum FileHandle::OpenMode pMode)
{
  if (isOpened()) {
    setState(BadBit);
    return false;
  }

  m_Handler = pFD;
  m_OpenMode = pMode;
  m_State = GoodBit;

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

  if (-1 == ::close(m_Handler)) {
    setState(FailBit);
    return false;
  }

  m_Size = 0;
  m_OpenMode = NotOpen;
  cleanState();
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
  return (m_State == GoodBit);
}

bool FileHandle::isBad() const
{
  return (m_State & BadBit);
}

bool FileHandle::isFailed() const
{
  return (m_State & BadBit || m_State & FailBit);
}

