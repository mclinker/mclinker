/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDFile.h>
#include "ARMELFSectLinker.h"

using namespace mcld;

ARMELFSectLinker::ARMELFSectLinker(MCLDInfo& pLDInfo,
                                   TargetLDBackend &pLDBackend)
  : SectLinker(pLDInfo, pLDBackend) {
}

ARMELFSectLinker::~ARMELFSectLinker()
{
}

