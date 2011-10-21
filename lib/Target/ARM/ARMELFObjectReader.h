//===- ARMELFObjectReader.h -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARMELFOBJECTREADER_H
#define ARMELFOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/MC/MCELFObjectTargetReader.h"

namespace mcld
{

/** \class ARMELFObjectReader
 *  \brief ARMELFObjectReader is a target-dependent ELF object reader.
 */
class ARMELFObjectReader : public MCELFObjectTargetReader
{
public:
  virtual bool hasRelocationAddend();
  virtual unsigned getRelocType(const MCValue&,
                                const MCFixup&,
                                bool IsPCRel,
                                bool IsRelocWithSymbol,
                                int64_t);
  virtual const MCSymbol* explicitRelSym(const MCAssembler&,
                                         const MCValue&,
                                         const MCFragment&,
                                         const MCFixup&,
                                         bool) const;
};

} // namespace of mcld

#endif

