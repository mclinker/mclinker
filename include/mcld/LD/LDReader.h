//===- LDReader.h ---------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_READER_INTERFACE_H
#define MCLD_READER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class Input;

/** \class LDReader
 *  \brief LDReader provides the basic interfaces for all readers. It also
 *  provides basic functions to read data stream.
 */
class LDReader
{
public:
  enum Endian {
    LittleEndian,
    BigEndian
  };

protected:
  LDReader() { }

public:
  virtual ~LDReader() { }

  virtual bool isMyFormat(Input& pInput) const = 0;

  virtual Endian endian(Input& pFile) const = 0;

};

} // namespace of mcld

#endif

