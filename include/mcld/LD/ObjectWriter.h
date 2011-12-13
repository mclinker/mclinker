//===- ObjectWriter.h -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OBJECT_WRITER_INTERFACE_H
#define MCLD_OBJECT_WRITER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class TargetLDBackend;

/** \class ObjectWriter
 *  \brief ObjectWriter provides a common interface for object file writers.
 */
class ObjectWriter
{
protected:
  ObjectWriter(TargetLDBackend& pBackend);

public:
  virtual ~ObjectWriter();

  TargetLDBackend& target()
  { return m_LDBackend; }

  const TargetLDBackend& target() const
  { return m_LDBackend; }

private:
  TargetLDBackend& m_LDBackend;
};

} // namespace of mcld

#endif

