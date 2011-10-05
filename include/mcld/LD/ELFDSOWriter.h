/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com> (owner)                                  *
 ****************************************************************************/

#ifndef MCLD_LD_ELFDSOWRITER
#define MCLD_LD_ELFDSOWRITER

#include <mcld/LD/LDELFWriter.h>

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
