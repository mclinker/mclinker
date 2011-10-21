//===- MCELFObjectTargetWriter.h ------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCELFOBJECTTARGETWRITER_H
#define MCELFOBJECTTARGETWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCELFObjectTargetWriter
 *  \brief MCELFObjectTargetWriter provides an abstract interface for all target ELF object writer.
 *
 *  \see
 */
class MCELFObjectTargetWriter
{
protected:
  MCELFObjectTargetWriter();

public:
  virtual ~MCELFObjectTargetWriter();
};

} // namespace of mcld

#endif

