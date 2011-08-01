/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_ARM_H
#define MCLD_ARM_H
#include <string>
#include <mcld/Target/TargetMachine.h>

namespace mcld {
class TargetLDBackend;

extern mcld::Target TheARMTarget;

TargetLDBackend *createARMLDBackend(const llvm::Target&, const std::string&);

} // namespace of mcld

#endif

