//===- ELFDSOWriter.h -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LD_ELFDSOWRITER
#define MCLD_LD_ELFDSOWRITER

#include "mcld/LD/LDELFWriter.h"

class MemoryArea;

namespace mcld {
class ELFDSOWriter : private LDELFWriter {
public:
  ELFDSOWriter(MemoryArea *Area, bool _IsLittleEndian)
  : LDELFWriter(Area, _IsLittleEndian) {}

  void writeFile();

private:
  void writeHeader();
};
} // end namespace mcld
#endif
