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
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/StringSwitch.h>

namespace mcld {

class HexagonGNUInfo : public GNUInfo
{
  enum CPUType {
    V3 = 0x2,
    V4 = 0x3,
    V5 };
  const llvm::TargetMachine* m_pTargetMachine;

public:
  HexagonGNUInfo(const llvm::Triple& pTriple, const llvm::TargetMachine* pTM)
    : GNUInfo(pTriple),
      m_pTargetMachine(pTM) { }

  uint32_t machine() const { return llvm::ELF::EM_HEXAGON; }

  uint64_t defaultTextSegmentAddr() const { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  uint64_t flags() const {
    return llvm::StringSwitch<uint64_t>(m_pTargetMachine->getTargetCPU())
      .Case("hexagonv4", V4)
      .Case("hexagonv5", V5)
      .Default(V4);
  }
};

} // namespace of mcld

#endif

