//===- DynObjWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_DYNAMIC_SHARED_OBJECT_WRITER_H
#define MCLD_DYNAMIC_SHARED_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/LDWriter.h>
#include <llvm/Support/system_error.h>

namespace mcld {

class Module;
class MemoryArea;
class TargetLDBackend;

/** \class DynObjWriter
 *  \brief DynObjWriter provides an common interface for different object
 *  formats.
 */
class DynObjWriter : public LDWriter
{
protected:
  // force to have a TargetLDBackend
  DynObjWriter(TargetLDBackend& pLDBackend)
  { }

public:
  virtual ~DynObjWriter() { }

  virtual llvm::error_code writeDynObj(Module& pModule,
                                       MemoryArea& pOutput) = 0;
};

} // namespace of mcld

#endif

