/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/ADT/PZString.h>

using namespace mcld;

//==========================
// PZString
PZString::PZString()
{
}

PZString::~PZString()
{
}

PZString::iterator PZString::rbegin()
{
}

PZString::iterator PZString::rend()
{
}

PZString::const_iterator PZString::rbegin() const
{
}

PZString::const_iterator PZString::rend() const
{
}

const char& PZString::at(PZString::size_type pPos) const
{
}

char& PZString::at(PZString::size_type pPos)
{
}

PZString& PZString::assign(const char* pStr, PZString::Alloc& pFactory)
{
}

PZString& PZString::assign(const char* pStr, size_type pLength, PZString::Alloc& pFactory)
{
}

PZString& PZString::assign(const char* pStart, const char* pLast, PZString::Alloc& pFactory)
{
}

PZString& PZString::assign(const std::string& pStr, PZString::Alloc& pFactory)
{
}

PZString& PZString::assign(const llvm::StringRef& pStr, PZString::Alloc& pFactory)
{
}

PZString& PZString::assign(const PZString& pCopy, PZString::Alloc& pFactory)
{
}

