//===- X86ELFDynamic.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_ELFDYNAMIC_SECTION_H
#define MCLD_X86_ELFDYNAMIC_SECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/ELFDynamic.h>

namespace mcld {

class X86ELFDynamic : public ELFDynamic
{
public:
  X86ELFDynamic(const GNULDBackend& pParent);
  ~X86ELFDynamic();

private:
  void reserveTargetEntries(const ELFFileFormat& pFormat);
  void applyTargetEntries(const ELFFileFormat& pFormat);

private:
  // True if we have .got.plt section, which will avoid GOT0 entries
  // when PLT isn't used.  To support .got.plt section, we must combine
  // .got section and .got.plt section into a single GOT.
  bool m_HasGOTPLT;
};

} // namespace of mcld

#endif
