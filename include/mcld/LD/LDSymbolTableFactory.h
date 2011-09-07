/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTABLEFACTORY_H
#define LDSYMBOLTABLEFACTORY_H
#include <mcld/MC/MCLDInput.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDSymbolTableFactory
 *  \brief LDSymbolTableFactory constructs the SymbolTables.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
template<typename bool global>
class LDSymbolTableFactory
{
  /* draft. */
public:
  LDSymbolTableFactory();
  LDIOSymbolTableIF *Create(StringTable *, const Input&);
  LDIOSymbolTableIF *Create(StringTable *, const Output&);
  LDSymbolTableIF *Create(StringTable *);
};

} // namespace of mcld

#endif

