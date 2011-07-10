/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/CodeGen/SectLinker.h>
#include "ARMELFSectLinker.h"

using namespace mcld;

ARMELFSectLinker::ARMELFSectLinker(llvm::AsmPrinter &pPrinter,
                                   TargetLDBackend &pLDBackend)
  : SectLinker(pPrinter, pLDBackend) {
}

ARMELFSectLinker::~ARMELFSectLinker()
{
}

