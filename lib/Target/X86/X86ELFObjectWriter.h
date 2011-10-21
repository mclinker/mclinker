//===- X86ELFObjectWriter.h -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86ELFOBJECTWRITER_H
#define X86ELFOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/MC/MCELFObjectTargetWriter.h"

namespace mcld
{

/** \class X86ELFObjectWriter
 *  \brief X86ELFObjectWriter writes target-dependent parts of ELF object file.
 *
 *  \see
 */
class X86ELFObjectWriter : public MCELFObjectTargetWriter
{

};

} // namespace of mcld

#endif

