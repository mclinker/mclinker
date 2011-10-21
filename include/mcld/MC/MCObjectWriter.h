//===- MCObjectWriter.h ---------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCOBJECTWRITER_H
#define MCOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCObjectWriter
 *  \brief MCObjectWriter provides a common interface for object file writers.
 *
 *  \see
 */
class MCObjectWriter
{
protected:
  MCObjectWriter();

public:
  virtual ~MCObjectWriter();
};

} // namespace of mcld

#endif

