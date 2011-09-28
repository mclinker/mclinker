/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_SYMBOL_RESOLVER_H
#define MCLD_SYMBOL_RESOLVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;

/** \class Resolver
 *  \brief Resolver binds a symbol reference from one file to a symbol
 *   definition of another file.
 *
 *  Resolver seals up the algorithm of symbol resolution. The resolution of
 *  two symbols depends on their type, binding and whether it is belonging to
 *  a shared object.
 */
class Resolver
{
public:
  /// shouldOverwrite - Can resolver overwrite the symbol pOld by the
  /// symbol pNew?
  //  @param pOld the symbol which may be overwrited.
  //  @param pNew the symbol which is used to replace pOld
  bool shouldOverwrite(const LDSymbol& pOld, const LDSymbol& pNew);
};

} // namespace of mcld

#endif

