//===- MCLDOptions.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDOptions.h"
#include "mcld/MC/MCLDInput.h"

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

