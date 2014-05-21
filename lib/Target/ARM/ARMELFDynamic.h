//===- ARMELFDynamic.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_ARM_ARMELFDYNAMIC_H
#define TARGET_ARM_ARMELFDYNAMIC_H

#include <mcld/Target/ELFDynamic.h>

namespace mcld {

class ARMELFDynamic : public ELFDynamic {
public:
  ARMELFDynamic(const GNULDBackend& pParent, const LinkerConfig& pConfig);
  ~ARMELFDynamic();

private:
  void reserveTargetEntries(const ELFFileFormat& pFormat);
  void applyTargetEntries(const ELFFileFormat& pFormat);
};

} // namespace of mcld

#endif
