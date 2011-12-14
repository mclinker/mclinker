/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Anders Cheng <Anders.Cheng@mediatek.com>                                *
 ****************************************************************************/

#include <mcld/Target/AndroidSectLinker.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDFile.h>
#include "ARMAndroidSectLinker.h"

using namespace mcld;

ARMAndroidSectLinker::ARMAndroidSectLinker(const std::string &pInputFilename,
                                           const std::string &pOutputFilename,
                                           unsigned int pOutputLinkType,
                                           MCLDInfo& pLDInfo,
                                           TargetLDBackend &pLDBackend)
  : AndroidSectLinker(pInputFilename,
                      pOutputFilename,
                      pOutputLinkType,
                      pLDInfo,
                      pLDBackend) {
  // set up target-dependent constraints of attibutes
  pLDInfo.attrFactory().constraint().disableWholeArchive();
  pLDInfo.attrFactory().constraint().disableAsNeeded();
  pLDInfo.attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  pLDInfo.attrFactory().predefined().unsetWholeArchive();
  pLDInfo.attrFactory().predefined().setDynamic();

}

ARMAndroidSectLinker::~ARMAndroidSectLinker()
{
}

