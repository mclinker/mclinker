/*****************************************************************************
 *   The mcld Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#include <mcld/MC/MCLDDriver.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDInfo.h>

using namespace mcld;

MCLDDriver::MCLDDriver(const MCLDInfo &pInfo, TargetLDBackend& pLDBackend)
  : m_LDBackend(pLDBackend) {
}

MCLDDriver::~MCLDDriver()
{
}

void MCLDDriver::addInputFile(StringRef pFilename)
{
}

void MCLDDriver::enterGroup()
{
}

void MCLDDriver::leaveGroup()
{
}

