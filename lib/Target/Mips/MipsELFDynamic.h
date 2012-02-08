//===- MipsELFDynamic.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_Mips_ELFDYNAMIC_SECTION_H
#define MCLD_Mips_ELFDYNAMIC_SECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/ELFDynamic.h>

namespace mcld {

class MipsELFDynamic : public ELFDynamic
{
public:
  MipsELFDynamic(const GNULDBackend& pParent);
  ~MipsELFDynamic();

private:
  void reserveTargetEntries(const ELFFileFormat& pFormat);
  void applyTargetEntries(const ELFFileFormat& pFormat);

  uint64_t getSymTabNo(const ELFFileFormat& pFormat) const;
};

} // namespace of mcld

#endif
