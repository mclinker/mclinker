//===- AArch64GNUInfo.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_AARCH64_GNU_INFO_H
#define MCLD_TARGET_AARCH64_GNU_INFO_H
#include <mcld/Target/GNUInfo.h>

#include <llvm/Support/ELF.h>

namespace mcld {

class AArch64GNUInfo : public GNUInfo
{
public:
  AArch64GNUInfo(const llvm::Triple& pTriple) : GNUInfo(pTriple) { }

  uint32_t machine() const { return llvm::ELF::EM_AARCH64; }

  // FIXME: the default segment address
  uint64_t defaultTextSegmentAddr() const { return 0x10000; }

  // FIXME: check if the flag is the same as arm
  uint64_t flags() const { return llvm::ELF::EF_ARM_EABI_VER5; }
};

} // namespace of mcld

#endif

