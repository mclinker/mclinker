//===- FileHandle.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_FILE_HANDLE_H
#define MCLD_FILE_HANDLE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/Path.h>

namespace mcld
{

/** \class FileHandle
 *  \brief FileHandle 
 */
class FileHandle
{
public:
  enum IOState
  {
    GoodBit    = 0,       // no error
    BadBit     = 1L << 0, // fail on the Space
    EOFBit     = 1L << 1, // reached End-Of-File
    FailBit    = 1L << 2, // internal logic fail
    IOStateEnd = 1L << 16
  };

  enum OpenMode
  {
    NotOpen   = 0x00,
    ReadOnly  = 0x01,
    WriteOnly = 0x02,
    ReadWrite = ReadOnly | WriteOnly,
    Append    = 0x04,
    Create    = 0x08,
    Truncate  = 0x10,
  };

  enum Permission
  {
    ReadUser    = 0x4000,
    WriteUser   = 0x2000,
    ExeUser     = 0x1000,
    ReadOwner   = 0x0400,
    WriteOwner  = 0x0200,
    ExeOwner    = 0x0100,
    ReadGroup   = 0x0040,
    WriteGroup  = 0x0020,
    ExeGroup    = 0x0010,
    ReadOther   = 0x0004,
    WriteOther  = 0x0002,
    ExeOther    = 0x0001,
  };

public:
  FileHandle();

  ~FileHandle();

  bool open(const sys::fs::Path& pPath, enum OpenMode pMode);

  bool open(const sys::fs::Path& pPath,
            enum OpenMode pMode,
            enum Permission pPerm);

  bool delegate(int& pFD, enum OpenMode pMode);

  void close();

  void setState(IOState pState);

  void clear(IOState pState = GoodBit);

  // -----  observers  ----- //
  const sys::fs::Path& path() const;

  size_t size() const;

  bool isOpened() const;

  bool isGood() const;

  bool isBad() const;

  bool isFailed() const;

  bool isReadable() const;

  bool isWritable() const;

  bool isReadWrite() const;

  int rdstate() const;
  
private:
  sys::fs::Path m_Path;
  int m_FD;
  unsigned int m_Size;
  uint16_t m_AccessFlags;
  uint16_t m_State;
};

} // namespace of mcld

#endif

