/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_ARM_TARGET_MACHINE_H
#define MCLD_ARM_TARGET_MACHINE_H
#include <mcld/Target/TargetMachine.h>
#include "ARM.h"

namespace mcld
{

class ARMBaseTargetMachine : public LLVMTargetMachine
{
protected:
  MCLDInfo *m_pLDInfo;

public:
  ARMBaseTargetMachine(llvm::TargetMachine &pTM,
                       const mcld::Target &pTarget,
                       const std::string &pTriple);

  virtual ~ARMBaseTargetMachine();

  mcld::MCLDInfo& getLDInfo()
  { return *m_pLDInfo; }

  const mcld::MCLDInfo& getLDInfo() const
  { return *m_pLDInfo; }

};

} // namespace of mcld

#endif

