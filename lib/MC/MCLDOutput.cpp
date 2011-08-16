/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDOutput.h>

using namespace mcld;

//==========================
// MCLDOutput
Output::Output()
  : MCLDFile("output") {
}

Output::Output(const sys::fs::Path& pRealPath,
               Output::Type pType)
  : MCLDFile("output", pRealPath, pType) {
}
                  
Output::~Output()
{
}

