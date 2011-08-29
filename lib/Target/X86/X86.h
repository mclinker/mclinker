/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_X86_H
#define MCLD_X86_H
#include <string>
#include <mcld/Target/TargetMachine.h>

namespace mcld {
class TargetLDBackend;

extern mcld::Target TheX86Target;

TargetLDBackend *createX86LDBackend(const llvm::Target&, const std::string&);

} // namespace of mcld

#endif

