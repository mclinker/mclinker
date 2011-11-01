//===- MCObjectReader.h ---------------------------------------------------===//
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

#include "mcld/MC/MCLDInput.h"
#include "llvm/Support/system_error.h"

namespace mcld
{
class MCFixup;
class MCObjectTargetReader;
class MCSymbol;
class MCValue;

//MCObjectReader provides an interface for different object formats.
class MCObjectReader
{
protected:
  MCObjectReader() {}
public:
  virtual ~MCObjectReader(){}

  virtual bool isMyFormat(mcld::Input &pFile) const = 0;
  virtual Input::Type fileType(mcld::Input &pFile) const = 0;
  virtual llvm::error_code readObject(const std::string &ObjectFile,
                                      MCLDFile &) = 0;
};

} // namespace of mcld

#endif

