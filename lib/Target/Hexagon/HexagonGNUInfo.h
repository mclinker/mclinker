//===- HexagonGNUInfo.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_HEXAGON_GNU_INFO_H
#define MCLD_TARGET_HEXAGON_GNU_INFO_H
#include <mcld/Target/GNUInfo.h>

#include <llvm/Support/ELF.h>

namespace mcld {

class HexagonGNUInfo : public GNUInfo
{
public:
  HexagonGNUInfo(const llvm::Triple& pTriple) : GNUInfo(pTriple) { }

  uint32_t machine() const { return llvm::ELF::EM_HEXAGON; }

  uint64_t defaultTextSegmentAddr() const { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  uint64_t flags() const { return 0x2; }
};

} // namespace of mcld

#endif

