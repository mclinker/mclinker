//===- LDELFWriter.h ------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// LDELFWriter provides the APIs which handles
// the common functionality used by ELFDSOWriter and ELFEXEWriter.
//
//===----------------------------------------------------------------------===//


#ifndef MCLD_LD_LDELFWRITER
#define MCLD_LD_LDELFWRITER

#include "mcld/LD/LDWriter.h"
#include "llvm/Support/ELF.h"

class MemoryArea;

namespace mcld {
class LDELFWriter : public LDWriter {
protected:
  LDELFWriter(MemoryArea *Area, bool _IsLittleEndian)
  : LDWriter(Area, _IsLittleEndian) {}

  virtual ~LDELFWriter() {};
};
} //end namespace
#endif
