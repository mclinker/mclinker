//===- ExecWriter.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_EXECUTABLE_OBJECT_WRITER_H
#define MCLD_EXECUTABLE_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDOutput.h>
#include <mcld/LD/LDWriter.h>
#include <llvm/Support/system_error.h>

namespace mcld
{

/** \class ExecWriter
 *  \brief ExecWriter provides an common interface for different object
 *  formats.
 */
class ExecWriter : public LDWriter
{
protected:
  // force to have a TargetLDBackend
  ExecWriter(TargetLDBackend& pLDBackend)
  { }

public:
  virtual ~ExecWriter() { }

  virtual llvm::error_code writeExecutable(Output& pOutput) = 0;
};

} // namespace of mcld

#endif

