//===- X86TargetMachine.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_TARGET_MACHINE_H
#define MCLD_X86_TARGET_MACHINE_H
#include "mcld/Target/TargetMachine.h"
#include "X86.h"

namespace mcld
{

class X86TargetMachine : public LLVMTargetMachine
{
protected:
  LinkerConfig *m_pConfig;

public:
  X86TargetMachine(llvm::TargetMachine &pTM,
                   const mcld::Target &pTarget,
                   const std::string &pTriple);

  virtual ~X86TargetMachine();

  const mcld::LinkerConfig& getConfig() const { return *m_pConfig; }
  mcld::LinkerConfig&       getConfig()       { return *m_pConfig; }

};

} // namespace of mcld

#endif

