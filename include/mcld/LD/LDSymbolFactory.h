/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef LD_SYMBOL_FACTORY_H
#define LD_SYMBOL_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/LDSymbol.h>

namespace mcld
{

/** \class LDSymbolFactory
 *  \brief LDSymbolFactory produces LDSymbols
 */
class LDSymbolFactory
{
public:
  typedef LDSymbol entry_type;
  typedef LDSymbol::key_type   key_type;

public:
  entry_type* produce(const key_type& pKey);
  void destroy(entry_type* pSymbol);
};

} // namespace of mcld

#endif

