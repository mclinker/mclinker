//===- ObjectReader.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OBJECT_READER_H
#define MCLD_OBJECT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/LD/LDReader.h"
#include <llvm/Support/system_error.h>

namespace mcld
{

class Input;
class TargetLDBackend;

/** \class ObjectReader
 *  \brief ObjectReader provides an common interface for different object
 *  formats.
 */
class ObjectReader : public LDReader
{
protected:
  ObjectReader(TargetLDBackend& pLDBackend)
  : m_TargetLDBackend(pLDBackend)
  { }

public:
  virtual ~ObjectReader() { }

  virtual llvm::error_code readObject(Input& pFile) = 0;

  virtual bool readSymbols(Input& pFile) = 0;

  virtual bool readSections(Input& pFile) = 0;

  inline TargetLDBackend& target()
  { return m_TargetLDBackend; }

  inline const TargetLDBackend& target() const
  { return m_TargetLDBackend; }
  
private:
  TargetLDBackend& m_TargetLDBackend;
};

} // namespace of mcld

#endif

