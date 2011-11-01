//===- MCELFObjectTargetReader.h ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_OBJECT_TARGET_READER_H
#define MCLD_ELF_OBJECT_TARGET_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/// class MCELFObjectTargetReader:
/// MCELFObjectTargetReader provides an abstract ELF interface
/// for target-dependent object readers.
class MCELFObjectTargetReader
{
public:
  virtual ~MCELFObjectTargetReader();

protected:
  MCELFObjectTargetReader();
};

} // namespace of mcld

#endif
