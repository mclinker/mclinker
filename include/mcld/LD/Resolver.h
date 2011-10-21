//===- Resolver.h ---------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
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
  enum OverrideAction {
    ERR_O, // Error, other.
    ERR_D, // Error, duplicate definition.
    OLD_W, // Use old symbol & WARNING.
    NEW_W, // Use new symbol & WARNING.
    OLD,   // Use old symbol.
    NEW,   // Use new symbol.
    OLD_B, // Use old symbol & set size to biggest.
    NEW_B, // Use new symbol & set size to biggest.

    NUMBER_OF_ACTION
  };
  /// shouldOverride - Can resolver override the symbol pOld by the symbol pNew?
  //  @param pOld the symbol which may be overridden.
  //  @param pNew the symbol which is used to replace pOld
  OverrideAction shouldOverride(const LDSymbol& pOld, const LDSymbol& pNew);
};

} // namespace of mcld

#endif

