/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   csmon7507 <csmon7507@gmail.com>                                         *
 ****************************************************************************/
#include <mcld/MC/MCLDInfo.h>
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

const char* MCLDInfo::sysroot() const
{
  return m_AbsSysRootPath.c_str();
}

void MCLDInfo::setSysroot(const std::string& pAbsPath)
{
  m_AbsSysRootPath = pAbsPath;
}

