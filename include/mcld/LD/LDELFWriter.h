/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com> (owner)                                  *
 ****************************************************************************/

#ifndef MCLD_LD_LDELFWRITER
#define MCLD_LD_LDELFWRITER

#include <mcld/LD/LDWriter.h>
#include <llvm/Support/ELF.h>

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
