//===- StaticResolver.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_STATIC_SYMBOL_RESOLVER_H
#define MCLD_STATIC_SYMBOL_RESOLVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include "mcld/LD/Resolver.h"

namespace mcld
{

class ResolveInfo;
class StrSymPool;

/** \class StaticResolver
 */
class StaticResolver : public Resolver
{
public:
  /** \enum LinkAction
   *  LinkAction follows BFD (binary file descriptor). List all actions
   *  to take in the state table
   */
  enum LinkAction
  {
    Fail,             /* Abort.  */
    MarkUndef,        /* Mark symbol undefined.  */
    MarkWeakUndef,    /* Mark symbol weak undefined.  */
    MarkDef,          /* Mark symbol defined.  */
    MarkWeakDef,      /* Mark symbol weak defined.  */
    MarkCom,          /* Mark symbol common.  */
    MarkInd,          /* Make indirect symbol.  */
    Resolved,         /* Mark defined symbol referenced.  */
    ComRefDef,        /* Possibly warn about common reference to defined symbol.  */
    DefRefCom,        /* Define existing common symbol.  */
    ComRefCom,        /* Mark symbol common using largest size.  */
    Noaction,         /* No action.  */
    DefRefDef,        /* Multiple definition error.  */
    IndRefInd,        /* Multiple indirect symbols.  */
    IndRefCom,        /* Make indirect symbol from existing common symbol.  */
    Cycle,            /* Repeat with symbol pointed to.  */
    REFC,         /* Mark indirect symbol referenced and then CYCLE.  */
    WARNC         /* Issue warning and then CYCLE.  */
  };

public:
  StaticResolver();

  virtual ~StaticResolver();

  /// shouldOverride - Can resolver override the symbol pOld by the symbol pNew?
  /// @return the action should be taken.
  /// @param pOld the symbol which may be overridden.
  /// @param pNew the symbol which is used to replace pOld
  virtual unsigned int resolve(const ResolveInfo & __restrict__ pOld,
                               const ResolveInfo & __restrict__ pNew,
                               bool &pOverride);

  /// resolveAgain - Can override by derived classes.
  /// @return if pStrSymPool is changed, return true. Otherwise, return false.
  /// @param pAction the action returned by resolve()
  virtual bool resolveAgain(StrSymPool& pStrSymPool,
                            unsigned int pAction,
                            const ResolveInfo& __restrict__ pOld,
                            const ResolveInfo& __restrict__ pNew)
  { return false; }
};

} // namespace of mcld

#endif

