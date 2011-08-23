/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDOptions.h>
#include <mcld/MC/MCLDInput.h>

using namespace mcld;

//==========================
// MCLDOptions
bool GeneralOptions::hasDefaultLDScript() const
{
}

const char* GeneralOptions::defaultLDScript() const
{
}

void GeneralOptions::setDefaultLDScript(const std::string& pFilename)
{
}

void GeneralOptions::setSysroot(const mcld::sys::fs::Path &pSysroot)
{
  m_Sysroot.assign(pSysroot);
}

