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
  // set attributes
  attrFactory()->reserve(32);
  MCLDAttribute* def_attr = attrFactory()->produce();  
  def_attr->setWholeArchive();
  def_attr->unsetAsNeeded();
  def_attr->setDynamic();

  attrFactory()->setDefault(*def_attr);
}

ARMELFSectLinker::~ARMELFSectLinker()
{
}

