//===- ELFObjectWriter.h --------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_OBJECT_WRITER_H
#define MCLD_ELF_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/LD/ObjectWriter.h"
#include "mcld/LD/ELFWriter.h"

namespace mcld
{

class Input;
class MCLinker;
class TargetLDBackend;

/** \class ELFObjectWriter
 *  \brief ELFObjectWriter writes the target-independent parts of object files.
 *  ELFObjectWriter reads a MCLDFile and writes into raw_ostream
 *
 */
class ELFObjectWriter : public ObjectWriter, protected ELFWriter
{
public:
  ELFObjectWriter(TargetLDBackend& pBackend, MCLinker& pLinker);

  ~ELFObjectWriter();

private:
  MCLinker& m_Linker;
};

} // namespace of mcld

#endif

