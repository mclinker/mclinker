//===- AArch64ELFDynamic.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_AARCH64_ELFDYNAMIC_SECTION_H
#define MCLD_AARCH64_ELFDYNAMIC_SECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/ELFDynamic.h>

namespace mcld {

class AArch64ELFDynamic : public ELFDynamic {
public:
  AArch64ELFDynamic(const GNULDBackend& pParent, const LinkerConfig& pConfig);
  ~AArch64ELFDynamic();

private:
  void reserveTargetEntries(const ELFFileFormat& pFormat);
  void applyTargetEntries(const ELFFileFormat& pFormat);
};

} // namespace of mcld

#endif
