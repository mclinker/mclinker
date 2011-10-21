//===- MCELFObjectWriter.h ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCELFOBJECTWRITER_H
#define MCELFOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/MC/MCObjectWriter.h"

namespace mcld
{
class MCELFObjectTargetWriter;

/** \class MCELFObjectWriter
 *  \brief MCELFObjectWriter writes the target-independent parts of object files.
 *  MCELFObjectWriter reads a MCLDFile and writes into raw_ostream
 *
 *  \see MCLDFile raw_ostream
 */
class MCELFObjectWriter : public MCObjectWriter
{
public:
  MCELFObjectWriter(const MCELFObjectTargetWriter *pTargetWriter);
  ~MCELFObjectWriter();

private:
  const MCELFObjectTargetWriter *m_pTargetWriter;
};

} // namespace of mcld

#endif

