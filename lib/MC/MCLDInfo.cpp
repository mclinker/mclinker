/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   csmon7507 <csmon7507@gmail.com>                                         *
 ****************************************************************************/
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/FileSystem.h>
#include <string>

using namespace mcld;

//==========================
// MCLDInfo
bool MCLDInfo::hasDefaultBitcode() const 
{
}

MCLDFile* MCLDInfo::defaultBitcode() const
{
}

void MCLDInfo::setDefaultBitcode(MCLDFile &pLDFile)
{
}

bool MCLDInfo::hasDefaultLDScript() const
{
}

const char* MCLDInfo::defaultLDScript() const
{
}

void MCLDInfo::setDefaultLDScript(const std::string& pFilename)
{
}

void MCLDInfo::setSysroot(const mcld::sys::fs::Path &pSysroot)
{
  m_Sysroot = pSysroot;
}

