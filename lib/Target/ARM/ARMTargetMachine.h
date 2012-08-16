//===- ARMTargetMachine.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_TARGET_MACHINE_H
#define MCLD_ARM_TARGET_MACHINE_H
#include "mcld/Target/TargetMachine.h"
#include "ARM.h"

namespace mcld
{

class ARMBaseTargetMachine : public LLVMTargetMachine
{
protected:
  LinkerConfig *m_pConfig;

public:
  ARMBaseTargetMachine(llvm::TargetMachine &pTM,
                       const mcld::Target &pTarget,
                       const std::string &pTriple);

  virtual ~ARMBaseTargetMachine();

  mcld::LinkerConfig& getConfig()
  { return *m_pConfig; }

  const mcld::LinkerConfig& getConfig() const
  { return *m_pConfig; }

};

} // namespace of mcld

#endif

