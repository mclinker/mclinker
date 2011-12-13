//===- Resolver.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
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
#include <string>

namespace mcld
{

class ResolveInfo;
class StrSymPool;

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
  enum Action {
    Success,
    Warning,
    Abort,
    LastAction
  };

public:
  Resolver();

  virtual ~Resolver();

  /// shouldOverride - Can resolver override the symbol pOld by the symbol pNew?
  /// @return the action should be taken.
  /// @param pOld the symbol which may be overridden.
  /// @param pNew the symbol which is used to replace pOld
  virtual unsigned int resolve(ResolveInfo & __restrict__ pOld,
                               const ResolveInfo & __restrict__ pNew,
                               bool &pOverride) = 0;

  /// resolveAgain - Can override by derived classes.
  /// @return if pStrSymPool is changed, return true. Otherwise, return false.
  /// @param pAction the action returned by resolve()
  virtual bool resolveAgain(StrSymPool& pStrSymPool,
                            unsigned int pAction,
                            ResolveInfo& __restrict__ pOld,
                            const ResolveInfo& __restrict__ pNew)
  { return false; }

  const std::string& mesg() const
  { return m_Mesg; }

  void clearMesg();

protected:
  std::string m_Mesg;

};

} // namespace of mcld

#endif

