//===- MipsTargetMachine.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_TARGET_MACHINE_H
#define MIPS_TARGET_MACHINE_H
#include "mcld/Target/TargetMachine.h"
#include "Mips.h"

namespace mcld
{

class MipsBaseTargetMachine : public LLVMTargetMachine
{
protected:
  MCLDInfo *m_pLDInfo;

public:
  MipsBaseTargetMachine(llvm::TargetMachine &pTM,
                        const mcld::Target &pTarget,
                        const std::string &pTriple);

  virtual ~MipsBaseTargetMachine();

  mcld::MCLDInfo& getLDInfo()
  { return *m_pLDInfo; }

  const mcld::MCLDInfo& getLDInfo() const
  { return *m_pLDInfo; }
};

} // namespace of mcld

#endif
