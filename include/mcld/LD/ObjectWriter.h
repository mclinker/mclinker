//===- ObjectWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_OBJECTWRITER_H
#define MCLD_LD_OBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Support/system_error.h>

namespace mcld {

class Module;
class MemoryArea;

/** \class ObjectWriter
 *  \brief ObjectWriter provides a common interface for object file writers.
 */
class ObjectWriter
{
protected:
  ObjectWriter();

public:
  virtual ~ObjectWriter();

  virtual llvm::error_code writeObject(Module& pModule, MemoryArea& pOutput) = 0;
};

} // namespace of mcld

#endif

