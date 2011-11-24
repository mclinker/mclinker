/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/ADT/PZStringFactory.h>
#include <mcld/ADT/PZString.h>
#include <cstdlib>

using namespace mcld;

//==========================
// PZStringFactory
PZString* PZStringFactory::create(const char* pStr)
{
}

PZString* PZStringFactory::create(const char* pStr, size_t pLength)
{
}

PZString* PZStringFactory::create(const char* pStart, const char* pLast)
{
}

PZString* PZStringFactory::create(const std::string& pStr)
{
}

PZString* PZStringFactory::create(const llvm::StringRef& pStr)
{
}

void PZStringFactory::destroy(PZString* pString)
{
}

