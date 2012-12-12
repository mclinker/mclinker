//===- ARMGNUInfo.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_ARM_GNU_INFO_H
#define MCLD_TARGET_ARM_GNU_INFO_H
#include <mcld/Target/GNUInfo.h>

#include <llvm/Support/ELF.h>

namespace mcld {

class ARMGNUInfo : public GNUInfo
{
public:
  uint32_t machine() const { return llvm::ELF::EM_ARM; }

};

} // namespace of mcld

#endif

