/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef MCLD_LD_SYMBOL_H
#define MCLD_LD_SYMBOL_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDSymbol
 *  \brief
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
class LDSymbol
{
// FIXME(Nowar): This is an coarse-grained scaffolding of LDSymbol.
//               It makes my compiler happy when buillding SymbolTable.
public:
  LDSymbol()
  : m_pStr(NULL) {}

  const char* m_pStr;
};

bool operator<(const LDSymbol& pLHS, const LDSymbol& pRHS)
{
  // TODO
  return true;
}

} // namespace mcld

#endif
