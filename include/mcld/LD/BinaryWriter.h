//===- BinaryWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_BINARY_WRITER_INTERFACE_H
#define MCLD_BINARY_WRITER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Support/system_error.h>

namespace mcld {

class Module;
class MemoryArea;
class GNULDBackend;

/** \class BinaryWriter
 *  \brief BinaryWriter provides a common interface for Binary file writers.
 */
class BinaryWriter
{
protected:
  BinaryWriter(GNULDBackend& pBackend);

public:
  virtual ~BinaryWriter();

  virtual llvm::error_code writeBinary(Module& pModule, MemoryArea& pOutput) = 0;
};

} // namespace of mcld

#endif

