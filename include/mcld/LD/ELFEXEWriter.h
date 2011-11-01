//===- ELFEXEWriter.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LD_ELFEXEWRITER
#define MCLD_LD_ELFEXEWRITER

#include "mcld/LD/LDELFWriter.h"

class MemoryArea;

namespace mcld {
class ELFEXEWriter : public LDELFWriter {
public:
  ELFEXEWriter(MemoryArea *Area, bool _IsLittleEndian)
  : LDELFWriter(Area, _IsLittleEndian) {}

  void writeFile();

private:
  void writeHeader();
};
} // end namespace mcld
#endif

